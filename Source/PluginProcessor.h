
#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"
#include "SynthVoice.h"
#include "SynthSound.h"
#include "FXChain.h"

class RadioSauceSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    RadioSauceSynthAudioProcessor();
    ~RadioSauceSynthAudioProcessor() override = default;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "RadioSauceSynth"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Access to parameters
    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;

    // Public helpers for the editor
    void triggerNewSauce() { needNewSauce.store(true); }
    void setStyle(int styleIndex) { applyStyle(styleIndex); }

private:
    FXChain fx;
    std::atomic<bool> needNewSauce { false };

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void applyStyle(int styleIndex);
    void randomizeSauce();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioSauceSynthAudioProcessor)
};
