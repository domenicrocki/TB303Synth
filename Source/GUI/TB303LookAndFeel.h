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
                              bool isHighlighted, bool isDown) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool isHighlighted, bool isDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height,
                      bool isButtonDown, int bx, int by, int bw, int bh,
                      juce::ComboBox& box) override;

    // Helpers
    static void paintSilverBg(juce::Graphics& g, juce::Rectangle<float> b, bool darker = false);
    static void paintDarkBg(juce::Graphics& g, juce::Rectangle<float> b);
    static void paintMetalKnob(juce::Graphics& g, float cx, float cy, float radius, float angle);
    static void paintLargeKnob(juce::Graphics& g, float cx, float cy, float radius,
                                float pos, float startA, float endA);
    static void paintLED(juce::Graphics& g, float x, float y, float sz, bool on);
    static void paint3DButton(juce::Graphics& g, juce::Rectangle<float> b, bool pressed = false);

    static juce::Colour silver()   { return juce::Colour(0xFFD0D0D4); }
    static juce::Colour silverLt() { return juce::Colour(0xFFE2E2E6); }
    static juce::Colour silverDk() { return juce::Colour(0xFFB8B8BC); }
    static juce::Colour panelDk()  { return juce::Colour(0xFF2A2A30); }
    static juce::Colour panelMd()  { return juce::Colour(0xFF3C3C44); }
    static juce::Colour textDk()   { return juce::Colour(0xFF1A1A1A); }
    static juce::Colour textLt()   { return juce::Colour(0xFFD0D0D8); }
    static juce::Colour ledOn()    { return juce::Colour(0xFFFF2020); }
    static juce::Colour ledOff()   { return juce::Colour(0xFF601818); }
    static juce::Colour accentOr() { return juce::Colour(0xFFE06020); }
    static juce::Colour dispBg()   { return juce::Colour(0xFFECECF0); }
};
