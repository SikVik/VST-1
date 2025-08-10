
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>

//==============================================================================
RadioSauceSynthAudioProcessor::RadioSauceSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "PARAMS", createLayout())
#endif
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SynthVoice(apvts));
    synth.addSound (new SynthSound());
}

void RadioSauceSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    fx.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

bool RadioSauceSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void RadioSauceSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Style & random button handling (stateless trigger)
    if (needNewSauce.exchange(false))
        randomizeSauce();

    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    // FX / Master params
    auto chorusMix = apvts.getRawParameterValue(IDs::chorusMix)->load();
    auto delayTime = apvts.getRawParameterValue(IDs::delayTime)->load();
    auto delayFdbk = apvts.getRawParameterValue(IDs::delayFdbk)->load();
    auto delayMix  = apvts.getRawParameterValue(IDs::delayMix)->load();
    auto reverbMix = apvts.getRawParameterValue(IDs::reverbMix)->load();
    auto crushAmt  = apvts.getRawParameterValue(IDs::crushAmt)->load();
    auto compAmt   = apvts.getRawParameterValue(IDs::compAmt)->load();
    auto width     = apvts.getRawParameterValue(IDs::width)->load();
    bool limitOn   = apvts.getRawParameterValue(IDs::limitOn)->load() > 0.5f;

    fx.setParams(chorusMix, delayTime, delayFdbk, delayMix, reverbMix, crushAmt, compAmt, width, limitOn);
    fx.processBlock(buffer);
}

void RadioSauceSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void RadioSauceSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto v = juce::ValueTree::readFromData(data, (size_t) sizeInBytes);
    if (v.isValid())
        apvts.replaceState(v);
}

juce::AudioProcessorValueTreeState::ParameterLayout RadioSauceSynthAudioProcessor::createLayout()
{
    using R = juce::AudioParameterFloatAttributes;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto f = [](float a, float b, float d, const char* name){ 
        return std::make_unique<juce::AudioParameterFloat>(name, name, juce::NormalisableRange<float>(a,b, d), (a+b)/2.0f); };

    params.push_back(f(0,1,0.001, IDs::oscMorph));
    params.push_back(f(0,1,0.001, IDs::subLevel));
    params.push_back(f(0,1,0.001, IDs::noiseLevel));
    params.push_back(std::make_unique<juce::AudioParameterInt>(IDs::unison, IDs::unison, 1, 7, 1));
    params.push_back(f(0,50,0.01, IDs::detune));
    params.push_back(f(0,1,0.001, IDs::spread));
    params.push_back(f(0,1,0.001, IDs::fmAmount));
    params.push_back(f(0,1,0.001, IDs::drive));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(IDs::filterType, IDs::filterType, juce::StringArray{"LP","BP","HP"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(IDs::cutoff, IDs::cutoff,
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.0f, 0.25f), 1200.0f));
    params.push_back(f(0.1,1.2,0.001, IDs::resonance));
    params.push_back(f(-1,1,0.001, IDs::filtEnvAmt));

    params.push_back(f(0.001, 2.0, 0.001, IDs::ampA));
    params.push_back(f(0.001, 2.0, 0.001, IDs::ampD));
    params.push_back(f(0.0,   1.0, 0.001, IDs::ampS));
    params.push_back(f(0.001, 2.0, 0.001, IDs::ampR));

    params.push_back(f(0.001, 2.0, 0.001, IDs::filA));
    params.push_back(f(0.001, 2.0, 0.001, IDs::filD));
    params.push_back(f(0.0,   1.0, 0.001, IDs::filS));
    params.push_back(f(0.001, 2.0, 0.001, IDs::filR));

    params.push_back(f(0,1,0.001, IDs::chorusMix));
    params.push_back(f(10,1500,1.0, IDs::delayTime));
    params.push_back(f(0,0.95,0.001, IDs::delayFdbk));
    params.push_back(f(0,1,0.001, IDs::delayMix));
    params.push_back(f(0,1,0.001, IDs::reverbMix));
    params.push_back(f(0,1,0.001, IDs::crushAmt));

    params.push_back(f(0,1,0.001, IDs::compAmt));
    params.push_back(f(0,1,0.001, IDs::width));
    params.push_back(std::make_unique<juce::AudioParameterBool>(IDs::limitOn, IDs::limitOn, true));

    params.push_back(f(0,1,0.001, IDs::macroBite));
    params.push_back(f(0,1,0.001, IDs::macroBody));
    params.push_back(f(0,1,0.001, IDs::macroAir));
    params.push_back(f(0,1,0.001, IDs::macroSpace));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(IDs::style, IDs::style, juce::StringArray{"Pop Gloss","Trap 808","R&B Silk"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(IDs::newSauce, IDs::newSauce, false));

    return { params.begin(), params.end() };
}

void RadioSauceSynthAudioProcessor::applyStyle(int styleIndex)
{
    // Set sensible defaults
    auto set = [&](const juce::String& id, float v){ apvts.getParameter(id)->beginChangeGesture(); apvts.getParameter(id)->setValueNotifyingHost(apvts.getParameter(id)->getNormalisableRange().convertTo0to1(v)); apvts.getParameter(id)->endChangeGesture(); };

    if (styleIndex == 0) // Pop Gloss
    {
        set(IDs::oscMorph, 0.3f);
        set(IDs::subLevel, 0.15f);
        set(IDs::noiseLevel, 0.05f);
        set(IDs::cutoff, 8000.0f);
        set(IDs::resonance, 0.15f);
        set(IDs::chorusMix, 0.25f);
        set(IDs::reverbMix, 0.18f);
        set(IDs::delayTime, 380.0f);
        set(IDs::delayMix, 0.15f);
        set(IDs::compAmt, 0.35f);
    }
    else if (styleIndex == 1) // Trap 808
    {
        set(IDs::oscMorph, 0.0f);
        set(IDs::subLevel, 0.7f);
        set(IDs::drive, 0.35f);
        set(IDs::filterType, 0.0f);
        set(IDs::cutoff, 180.0f);
        set(IDs::resonance, 0.25f);
        set(IDs::chorusMix, 0.0f);
        set(IDs::reverbMix, 0.0f);
        set(IDs::delayMix, 0.0f);
        set(IDs::compAmt, 0.55f);
    }
    else // R&B Silk
    {
        set(IDs::oscMorph, 0.5f);
        set(IDs::subLevel, 0.25f);
        set(IDs::noiseLevel, 0.03f);
        set(IDs::cutoff, 3200.0f);
        set(IDs::resonance, 0.2f);
        set(IDs::chorusMix, 0.35f);
        set(IDs::reverbMix, 0.28f);
        set(IDs::delayTime, 480.0f);
        set(IDs::delayMix, 0.22f);
        set(IDs::compAmt, 0.25f);
    }
}

void RadioSauceSynthAudioProcessor::randomizeSauce()
{
    std::mt19937 rng { std::random_device{}() };
    auto rf = [&](float a, float b){ std::uniform_real_distribution<float> d(a,b); return d(rng); };

    // Musical ranges (avoid harsh/inaudible)
    apvts.getParameter(IDs::oscMorph)->setValueNotifyingHost(rf(0.0f, 1.0f));
    apvts.getParameter(IDs::subLevel)->setValueNotifyingHost(rf(0.0f, 0.6f));
    apvts.getParameter(IDs::noiseLevel)->setValueNotifyingHost(rf(0.0f, 0.2f));
    apvts.getParameter(IDs::fmAmount)->setValueNotifyingHost(rf(0.0f, 0.3f));
    apvts.getParameter(IDs::drive)->setValueNotifyingHost(rf(0.0f, 0.5f));

    apvts.getParameter(IDs::filterType)->setValueNotifyingHost((float) (int) rf(0, 2.99f) / 2.0f);
    apvts.getParameter(IDs::cutoff)->setValueNotifyingHost(juce::NormalisableRange<float>(20.0f, 20000.0f, 0.0f, 0.25f).convertTo0to1(rf(120.0f, 9000.0f)));
    apvts.getParameter(IDs::resonance)->setValueNotifyingHost(rf(0.1f, 0.8f));
    apvts.getParameter(IDs::filtEnvAmt)->setValueNotifyingHost(rf(-0.4f, 0.7f));

    apvts.getParameter(IDs::chorusMix)->setValueNotifyingHost(rf(0.0f, 0.5f));
    apvts.getParameter(IDs::reverbMix)->setValueNotifyingHost(rf(0.0f, 0.35f));
    apvts.getParameter(IDs::delayTime)->setValueNotifyingHost(rf(120.0f, 600.0f));
    apvts.getParameter(IDs::delayFdbk)->setValueNotifyingHost(rf(0.1f, 0.7f));
    apvts.getParameter(IDs::delayMix)->setValueNotifyingHost(rf(0.0f, 0.35f));
    apvts.getParameter(IDs::crushAmt)->setValueNotifyingHost(rf(0.0f, 0.35f));
}

juce::AudioProcessorEditor* RadioSauceSynthAudioProcessor::createEditor()
{
    return new RadioSauceSynthAudioProcessorEditor(*this);
}


// This would be wired to a UI button in a custom editor; we still expose a bool param to trigger it
// We use the parameter in the Editor to call this; for Generic Editor you can automate it externally.
// (Left here for future GUI wiring.)
