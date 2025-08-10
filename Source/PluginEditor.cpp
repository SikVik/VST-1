
#include "PluginEditor.h"
#include "PluginProcessor.h"

RadioSauceSynthAudioProcessorEditor::RadioSauceSynthAudioProcessorEditor (RadioSauceSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      macroBite (p.apvts, IDs::macroBite,  "Bite"),
      macroBody (p.apvts, IDs::macroBody,  "Body"),
      macroAir  (p.apvts, IDs::macroAir,   "Air"),
      macroSpace(p.apvts, IDs::macroSpace, "Space"),
      oscMorph  (p.apvts, IDs::oscMorph,   "Morph"),
      subLevel  (p.apvts, IDs::subLevel,   "Sub"),
      cutoff    (p.apvts, IDs::cutoff,     "Cutoff"),
      reverbMix (p.apvts, IDs::reverbMix,  "Reverb"),
      delayMix  (p.apvts, IDs::delayMix,   "Delay"),
      compAmt   (p.apvts, IDs::compAmt,    "Glue"),
      width     (p.apvts, IDs::width,      "Width")
{
    setLookAndFeel(&lnf);
    setResizable(true, true);
    setSize (900, 520);

    title.setText("RadioSauce Synth", juce::dontSendNotification);
    title.setFont(juce::Font(26.0f, juce::Font::bold));
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    styleBox.addItem("Pop Gloss", 1);
    styleBox.addItem("Trap 808",  2);
    styleBox.addItem("R&B Silk",  3);
    addAndMakeVisible(styleBox);
    styleAttach.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(processor.apvts, IDs::style, styleBox));
    styleBox.onChange = [this]{
        processor.setStyle(styleBox.getSelectedId() - 1);
    };

    newSauceBtn.onClick = [this]{ processor.triggerNewSauce(); };
    addAndMakeVisible(newSauceBtn);

    limiterAttach.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(processor.apvts, IDs::limitOn, limiterToggle));
    addAndMakeVisible(limiterToggle);

    // Add components
    addAndMakeVisible(macroBite);
    addAndMakeVisible(macroBody);
    addAndMakeVisible(macroAir);
    addAndMakeVisible(macroSpace);

    addAndMakeVisible(oscMorph);
    addAndMakeVisible(subLevel);
    addAndMakeVisible(cutoff);
    addAndMakeVisible(reverbMix);
    addAndMakeVisible(delayMix);
    addAndMakeVisible(compAmt);
    addAndMakeVisible(width);

    startTimerHz(30);
}

void RadioSauceSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();

    // Background gradient with subtle grid
    juce::Colour top = juce::Colour::fromRGB(18,18,24);
    juce::Colour bottom = juce::Colour::fromRGB(10,10,14);
    g.setGradientFill(juce::ColourGradient(top, r.getCentreX(), r.getY(), bottom, r.getCentreX(), r.getBottom(), false));
    g.fillAll();

    // Neon frame
    g.setColour(juce::Colour::fromRGB(80,180,255).withAlpha(0.35f));
    g.drawRoundedRectangle(r.reduced(6), 16.0f, 4.0f);

    // Section titles
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawFittedText("STYLE", 20, 60, 100, 20, juce::Justification::left, 1);
    g.drawFittedText("MACROS", 20, 130, 100, 20, juce::Justification::left, 1);
    g.drawFittedText("TONE & FX", 20, 300, 120, 20, juce::Justification::left, 1);
}

void RadioSauceSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);

    // Top bar
    auto top = area.removeFromTop(48);
    title.setBounds(top.removeFromLeft(280));
    styleBox.setBounds(top.removeFromLeft(180).reduced(4, 8));
    newSauceBtn.setBounds(top.removeFromLeft(150).reduced(4, 8));
    limiterToggle.setBounds(top.removeFromLeft(120).reduced(4, 8));

    area.removeFromTop(8);

    // Macros row
    auto macroRow = area.removeFromTop(150);
    int macroW = macroRow.getWidth() / 4;
    macroBite.setBounds(macroRow.removeFromLeft(macroW).reduced(8));
    macroBody.setBounds(macroRow.removeFromLeft(macroW).reduced(8));
    macroAir.setBounds(macroRow.removeFromLeft(macroW).reduced(8));
    macroSpace.setBounds(macroRow.removeFromLeft(macroW).reduced(8));

    area.removeFromTop(8);

    // Tone & FX grid (2 rows x 4 cols)
    auto grid = area.removeFromTop(260);
    int cols = 4;
    int cellW = grid.getWidth() / cols;
    int cellH = grid.getHeight() / 2;

    oscMorph.setBounds(grid.removeFromLeft(cellW).removeFromTop(cellH).reduced(8));
    subLevel.setBounds((grid.withX(grid.getX()+cellW)).removeFromLeft(cellW).removeFromTop(cellH).reduced(8));
    cutoff.setBounds((grid.withX(grid.getX()+2*cellW)).removeFromLeft(cellW).removeFromTop(cellH).reduced(8));
    reverbMix.setBounds((grid.withX(grid.getX()+3*cellW)).removeFromLeft(cellW).removeFromTop(cellH).reduced(8));

    // Second row
    auto secondRow = area.removeFromTop(120); // not used, kept for spacing
    auto grid2 = juce::Rectangle<int>(16, 320, getWidth()-32, 120);
    int cW = (getWidth()-32) / 4;
    delayMix.setBounds(juce::Rectangle<int>(16 + 0*cW, 320, cW-16, 120).reduced(8));
    compAmt.setBounds (juce::Rectangle<int>(16 + 1*cW, 320, cW-16, 120).reduced(8));
    width.setBounds    (juce::Rectangle<int>(16 + 2*cW, 320, cW-16, 120).reduced(8));
}
