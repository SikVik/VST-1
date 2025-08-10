
#pragma once
#include <JuceHeader.h>

struct MorphOsc
{
    void prepare(double sr) { sampleRate = sr; phase = 0.0; }
    void setFrequency(float hz) { freq = hz; }
    void setMorph(float m) { morph = juce::jlimit(0.0f, 1.0f, m); }

    inline float process()
    {
        auto inc = juce::MathConstants<double>::twoPi * freq / sampleRate;
        phase += inc;
        if (phase > juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;

        // Base waves
        float s = std::sin((float)phase);                     // sine
        float saw = juce::jmap((float)phase, 0.0f, (float)juce::MathConstants<float>::twoPi, -1.0f, 1.0f); // naive saw
        float sq = s >= 0.0f ? 1.0f : -1.0f;                  // naive square

        // Crossfade Sine->Saw->Square
        float m = morph * 2.0f;
        float y;
        if (m <= 1.0f) y = juce::jmap(m, 0.0f, 1.0f, s, saw);
        else           y = juce::jmap(m - 1.0f, 0.0f, 1.0f, saw, sq);

        return y;
    }

    double sampleRate = 44100.0;
    double phase = 0.0;
    float freq = 100.0f;
    float morph = 0.0f;
};
