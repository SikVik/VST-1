
#pragma once
#include <JuceHeader.h>

class RadioSauceSynthAudioProcessor;

// Simple glossy LookAndFeel with rounded cards & neon-ish focus
class SauceLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SauceLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::white.withAlpha(0.95f));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(80,180,255));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB(30,30,35));
        setColour (juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(60,200,150));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB(24,24,28));
        setColour (juce::ComboBox::textColourId, juce::Colours::white);
        setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& s) override
    {
        auto r = juce::Rectangle<float>(x, y, width, height).reduced(6.0f);
        auto radius = juce::jmin(r.getWidth(), r.getHeight()) * 0.45f;
        auto cx = r.getCentreX();
        auto cy = r.getCentreY();
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Glossy background ring
        juce::Colour base = getColour(juce::Slider::rotarySliderFillColourId);
        juce::Colour glow = base.withAlpha(0.25f);
        g.setColour(glow);
        g.fillEllipse(cx - radius - 6.0f, cy - radius - 6.0f, (radius + 6.0f)*2, (radius + 6.0f)*2);

        juce::ColourGradient grad(base.brighter(0.6f), cx, cy - radius,
                                  base.darker(0.6f),  cx, cy + radius, false);
        g.setGradientFill(grad);
        g.fillEllipse(cx - radius, cy - radius, radius*2, radius*2);

        // Arc
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        juce::Path p;
        p.addCentredArc(cx, cy, radius*0.92f, radius*0.92f, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath(p, juce::PathStrokeType(3.0f));

        // Pointer
        auto pointerLength = radius * 0.8f;
        auto pointerThickness = 3.0f;
        juce::Path pointer;
        pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
        g.setColour(juce::Colours::white);
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(cx, cy));
    }
};

struct Knob : public juce::Component
{
    Knob(juce::AudioProcessorValueTreeState& s, const juce::String& id, const juce::String& labelText)
        : sliderAttachment(s, id, slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        label.setBounds(area.removeFromBottom(20));
        slider.setBounds(area.reduced(4));
    }

    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment sliderAttachment;
};

class RadioSauceSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    RadioSauceSynthAudioProcessorEditor (RadioSauceSynthAudioProcessor&);
    ~RadioSauceSynthAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override { repaint(); }

    RadioSauceSynthAudioProcessor& processor;
    SauceLookAndFeel lnf;

    // Top bar
    juce::Label title;
    juce::ComboBox styleBox;
    juce::TextButton newSauceBtn { "NEW SAUCE" };

    // Macro knobs
    Knob macroBite, macroBody, macroAir, macroSpace;

    // Core tone/FX knobs
    Knob oscMorph, subLevel, cutoff, reverbMix, delayMix, compAmt, width;

    // Toggle
    juce::ToggleButton limiterToggle { "Limiter" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> limiterAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioSauceSynthAudioProcessorEditor)
};
