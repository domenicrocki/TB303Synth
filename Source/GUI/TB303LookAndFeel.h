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

    // Colors
    static juce::Colour getSilverBackground() { return juce::Colour(0xFFD0D0D4); }
    static juce::Colour getDarkPanel() { return juce::Colour(0xFF2A2A30); }
    static juce::Colour getKnobColor() { return juce::Colour(0xFFC0C0C4); }
    static juce::Colour getLEDRed() { return juce::Colour(0xFFFF2020); }
    static juce::Colour getLEDOff() { return juce::Colour(0xFF602020); }
    static juce::Colour getTextColor() { return juce::Colour(0xFF1A1A1A); }
    static juce::Colour getAccentBlue() { return juce::Colour(0xFF4060A0); }
};
