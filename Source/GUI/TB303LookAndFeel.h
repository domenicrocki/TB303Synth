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

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // Futuristic color palette
    static juce::Colour getBgDark()      { return juce::Colour(0xFF0D0D12); }
    static juce::Colour getBgMid()       { return juce::Colour(0xFF1A1A24); }
    static juce::Colour getBgLight()     { return juce::Colour(0xFF252530); }
    static juce::Colour getPanelBg()     { return juce::Colour(0xFF14141C); }
    static juce::Colour getBorder()      { return juce::Colour(0xFF2A2A3A); }
    static juce::Colour getBorderLight() { return juce::Colour(0xFF3A3A4E); }

    // Neon accent colors
    static juce::Colour getNeonCyan()    { return juce::Colour(0xFF00E5FF); }
    static juce::Colour getNeonBlue()    { return juce::Colour(0xFF4060FF); }
    static juce::Colour getNeonPurple()  { return juce::Colour(0xFF8040FF); }
    static juce::Colour getNeonPink()    { return juce::Colour(0xFFFF2080); }
    static juce::Colour getNeonOrange()  { return juce::Colour(0xFFFF6020); }

    static juce::Colour getTextBright()  { return juce::Colour(0xFFE0E0F0); }
    static juce::Colour getTextDim()     { return juce::Colour(0xFF808098); }
    static juce::Colour getTextLabel()   { return juce::Colour(0xFF9898B0); }

    static juce::Colour getKnobBg()      { return juce::Colour(0xFF202030); }
    static juce::Colour getKnobRing()    { return juce::Colour(0xFF3A3A50); }
    static juce::Colour getLEDOn()       { return juce::Colour(0xFF00FF80); }
    static juce::Colour getLEDOff()      { return juce::Colour(0xFF1A3020); }
    static juce::Colour getLEDRed()      { return juce::Colour(0xFFFF2040); }

    // Panel drawing helpers
    static void drawFuturisticPanel(juce::Graphics& g, juce::Rectangle<float> bounds,
                                     juce::Colour accentColor = juce::Colour(0xFF00E5FF));
    static void drawGlowLine(juce::Graphics& g, float x1, float y1, float x2, float y2,
                               juce::Colour color, float thickness = 1.0f);
};
