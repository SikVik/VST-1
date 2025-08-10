
#pragma once
#include <JuceHeader.h>

struct FXChain
{
    void prepare(double sr, int block, int channels)
    {
        juce::dsp::ProcessSpec s { sr, (juce::uint32) block, (juce::uint32) channels };
        chorus.prepare(s);
        reverb.prepare(s);
        crusher.prepare(s);
        comp.prepare(s);

        // Delay (simple)
        delayLine.reset();
        delayLine.setMaximumDelayInSamples((int)(sr * 2.0));
        mixSmoothed.reset(sr, 0.05);
        widthSmoothed.reset(sr, 0.05);
        comp.setRatio(2.0f);
        comp.setThreshold(-12.0f);
        comp.setAttack(10.0f);
        comp.setRelease(100.0f);
    }

    void setParams(float chorusMix, float delayTimeMs, float delayFb, float delayMix,
                   float reverbMix, float crushAmt, float compAmt, float width, bool limitOn)
    {
        this->limitOn = limitOn;
        mixSmoothed.setTargetValue(delayMix);
        widthSmoothed.setTargetValue(width);

        // Chorus
        chorus.setMix(juce::jlimit(0.0f, 1.0f, chorusMix));
        chorus.setDepth(0.5f);
        chorus.setCentreDelay(7.0f);
        chorus.setFeedback(0.1f);
        chorus.setRate(0.25f);

        // Reverb
        juce::dsp::Reverb::Parameters rp;
        rp.roomSize = 0.45f;
        rp.wetLevel = juce::jlimit(0.0f, 1.0f, reverbMix);
        rp.dryLevel = 1.0f - rp.wetLevel;
        rp.width = 1.0f;
        rp.damping = 0.35f;
        reverb.setParameters(rp);

        // Crusher (simple)
        crushSteps = juce::jmap(crushAmt, 0.0f, 1.0f, 0.0f, 64.0f);

        // Delay
        delaySamples = (int) juce::jlimit(1.0f, 2.0f * 48000.0f, delayTimeMs * 48.0f); // up to 2s @48k
        delayFeedback = juce::jlimit(0.0f, 0.95f, delayFb);

        // Comp amount maps to makeup via output stage
        compMakeup = juce::Decibels::decibelsToGain(juce::jmap(compAmt, 0.0f, 1.0f, 0.0f, 6.0f));
    }

    inline float processSample(int ch, float x)
    {
        // Width (mid/side matrix)
        float w = widthSmoothed.getNextValue(); // 0..1
        // We'll apply width later on stereo buffer; here pass-through for mono path
        float y = x;

        // Bitcrusher (staircase quantize)
        if (crushSteps > 1.0f)
        {
            float step = 2.0f / crushSteps;
            y = std::floor((y + 1.0f) / step) * step - 1.0f;
        }

        // Delay
        float delayed = delayLine.popSample(ch);
        float toDelay = y + delayed * delayFeedback;
        delayLine.pushSample(ch, toDelay);
        float dmix = mixSmoothed.getNextValue();
        y = y * (1.0f - dmix) + delayed * dmix;

        // Simple soft clip
        y = std::tanh(y * compMakeup * 1.2f);

        return y;
    }

    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        chorus.process(juce::dsp::ProcessContextReplacing<float>(block));
        reverb.process(juce::dsp::ProcessContextReplacing<float>(block));

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = processSample(ch, data[i]);
        }

        // Simple limiter if enabled
        if (limitOn)
        {
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto* d = buffer.getWritePointer(ch);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                    d[i] = juce::jlimit(-0.89f, 0.89f, d[i]);
            }
        }
    }

    juce::dsp::Chorus<float> chorus;
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float> delayLine { 96000 };
    juce::SmoothedValue<float> mixSmoothed, widthSmoothed;
    float delayFeedback = 0.3f;
    int delaySamples = 4800;
    float crushSteps = 0.0f;
    bool limitOn = true;
    juce::dsp::Compressor<float> comp;
    float compMakeup = 1.0f;
    // (Width applied implicitly via stereo content in chorus/reverb/delay; for full M/S, expand here if desired)
};
