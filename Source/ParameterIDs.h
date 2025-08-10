
#pragma once
#include <JuceHeader.h>

namespace IDs {
    // Osc / Tone
    const juce::String oscMorph   = "oscMorph";   // 0..1: Sine->Saw->Square
    const juce::String subLevel   = "subLevel";   // 0..1
    const juce::String noiseLevel = "noiseLevel"; // 0..1
    const juce::String unison     = "unison";     // 1..7
    const juce::String detune     = "detune";     // cents 0..50
    const juce::String spread     = "spread";     // 0..1 stereo
    const juce::String fmAmount   = "fmAmount";   // 0..1
    const juce::String drive      = "drive";      // 0..1

    // Filter
    const juce::String filterType = "filterType"; // 0=LP,1=BP,2=HP
    const juce::String cutoff     = "cutoff";     // 20..20000
    const juce::String resonance  = "resonance";  // 0.1..1.2
    const juce::String filtEnvAmt = "filtEnvAmt"; // -1..1

    // Envelopes
    const juce::String ampA = "ampA", ampD = "ampD", ampS = "ampS", ampR = "ampR";
    const juce::String filA = "filA", filD = "filD", filS = "filS", filR = "filR";

    // FX
    const juce::String chorusMix = "chorusMix";
    const juce::String delayTime = "delayTime";
    const juce::String delayFdbk = "delayFdbk";
    const juce::String delayMix  = "delayMix";
    const juce::String reverbMix = "reverbMix";
    const juce::String crushAmt  = "crushAmt";

    // Master
    const juce::String compAmt   = "compAmt";
    const juce::String width     = "width";
    const juce::String limitOn   = "limitOn";

    // Macros
    const juce::String macroBite  = "macroBite";
    const juce::String macroBody  = "macroBody";
    const juce::String macroAir   = "macroAir";
    const juce::String macroSpace = "macroSpace";

    // UI
    const juce::String style     = "style";      // 0 Pop Gloss, 1 Trap 808, 2 R&B Silk
    const juce::String newSauce  = "newSauce";   // button trigger
}
