#pragma once
#include <JuceHeader.h>

class TB303LookAndFeel : public juce::LookAndFeel_V4
{
public:
    TB303LookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height,
                      bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH, juce::ComboBox& box) override;

    // Draw a large metallic knob (for Volume)
    static void drawLargeKnob(juce::Graphics& g, juce::Rectangle<float> bounds,
                               float sliderPos, float startAngle, float endAngle);

    // Draw the metallic panel background
    static void drawSilverPanel(juce::Graphics& g, juce::Rectangle<float> bounds,
                                 bool darker = false);

    // Draw dark sequencer panel
    static void drawDarkPanel(juce::Graphics& g, juce::Rectangle<float> bounds);

    // Colors
    static juce::Colour getSilverLight() { return juce::Colour(0xFFD8D8DC); }
    static juce::Colour getSilverMid() { return juce::Colour(0xFFC8C8CC); }
    static juce::Colour getSilverDark() { return juce::Colour(0xFFB0B0B4); }
    static juce::Colour getDarkPanel() { return juce::Colour(0xFF28282E); }
    static juce::Colour getDarkPanelLight() { return juce::Colour(0xFF3A3A42); }
    static juce::Colour getKnobBody() { return juce::Colour(0xFFD0D0D8); }
    static juce::Colour getKnobShadow() { return juce::Colour(0xFF707078); }
    static juce::Colour getLEDRed() { return juce::Colour(0xFFFF2020); }
    static juce::Colour getLEDOff() { return juce::Colour(0xFF602020); }
    static juce::Colour getTextDark() { return juce::Colour(0xFF1A1A1A); }
    static juce::Colour getTextLight() { return juce::Colour(0xFFCCCCD0); }
    static juce::Colour getAccentOrange() { return juce::Colour(0xFFE06020); }
    static juce::Colour getDisplayBg() { return juce::Colour(0xFFE8E8EC); }
};
