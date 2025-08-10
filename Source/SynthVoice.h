
#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"
#include "Wavetable.h"

struct SynthVoice : public juce::SynthesiserVoice
{
    SynthVoice(juce::AudioProcessorValueTreeState& s)
        : apvts(s)
    {
        filter.state->type = juce::dsp::StateVariableTPTFilterType::lowpass;
    }

    void prepareToPlay(double sr, int samplesPerBlock, int outputChannels)
    {
        sampleRate = sr;
        mainOsc.prepare(sr);
        subOsc.prepare(sr);

        juce::dsp::ProcessSpec spec { sr, (juce::uint32) samplesPerBlock, (juce::uint32) outputChannels };
        filter.prepare(spec);
        waveshaper.functionToUse = [](float x){ return std::tanh(x); };
        gain.prepare(spec);
        gain.setGainLinear(0.6f);

        ampEnv.setSampleRate(sr);
        filEnv.setSampleRate(sr);
    }

    bool canPlaySound (juce::SynthesiserSound* s) override { return dynamic_cast<SynthSound*>(s) != nullptr; }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
    {
        auto hz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        noteHz = hz;
        mainOsc.setFrequency(hz);
        subOsc.setFrequency(hz * 0.5f);

        ampEnv.noteOn();
        filEnv.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        ampEnv.noteOff();
        filEnv.noteOff();
        if (! allowTailOff) clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto& p = apvts;
        auto oscMorph   = p.getRawParameterValue(IDs::oscMorph)->load();
        auto subLevel   = p.getRawParameterValue(IDs::subLevel)->load();
        auto noiseLevel = p.getRawParameterValue(IDs::noiseLevel)->load();
        auto fmAmount   = p.getRawParameterValue(IDs::fmAmount)->load();
        auto drive      = p.getRawParameterValue(IDs::drive)->load();

        int filterMode  = (int) p.getRawParameterValue(IDs::filterType)->load();
        auto cutoff     = p.getRawParameterValue(IDs::cutoff)->load();
        auto resonance  = p.getRawParameterValue(IDs::resonance)->load();
        auto envAmt     = p.getRawParameterValue(IDs::filtEnvAmt)->load();

        // Envelopes
        ampEnv.setParameters({ p.getRawParameterValue(IDs::ampA)->load(),
                               p.getRawParameterValue(IDs::ampD)->load(),
                               p.getRawParameterValue(IDs::ampS)->load(),
                               p.getRawParameterValue(IDs::ampR)->load() });

        filEnv.setParameters({ p.getRawParameterValue(IDs::filA)->load(),
                               p.getRawParameterValue(IDs::filD)->load(),
                               p.getRawParameterValue(IDs::filS)->load(),
                               p.getRawParameterValue(IDs::filR)->load() });

        // Filter mode
        using FType = juce::dsp::StateVariableTPTFilterType;
        filter.state->type = (filterMode == 0 ? FType::lowpass : filterMode == 1 ? FType::bandpass : FType::highpass);

        while (numSamples--)
        {
            // FM simple: modulate freq by noise * amount
            float mod = ((rand() / (float) RAND_MAX) * 2.0f - 1.0f) * fmAmount * 15.0f; // +/- 15 Hz
            mainOsc.setMorph(oscMorph);
            mainOsc.setFrequency(noteHz + mod);

            subOsc.setMorph(0.0f); // sine-ish

            float s = mainOsc.process();
            float sub = subOsc.process() * subLevel;
            float n = ((rand() / (float) RAND_MAX) * 2.0f - 1.0f) * noiseLevel;

            float y = s + sub + n;

            // Filter envelope to cutoff
            float filEnvVal = filEnv.getNextSample();
            float targetCut = cutoff * std::pow(2.0f, envAmt * (filEnvVal - 0.5f));
            filter.setCutoffFrequency(targetCut);
            filter.setResonance(resonance);
            y = filter.processSample(0, y);

            // Drive -> waveshaper
            y = std::tanh(y * (1.0f + drive * 6.0f));

            // Amp envelope
            y *= ampEnv.getNextSample();

            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
                outputBuffer.addSample(ch, startSample, y);

            ++startSample;
        }
    }

    void setVoiceParamsFromAPVTS() {}

    juce::ADSR ampEnv, filEnv;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::Gain<float> gain;

    MorphOsc mainOsc, subOsc;
    double sampleRate = 44100.0;
    float noteHz = 100.0f;

    juce::AudioProcessorValueTreeState& apvts;
};
