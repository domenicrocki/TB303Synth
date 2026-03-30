#pragma once
#include <JuceHeader.h>

namespace TB303Colors
{
    inline juce::Colour background()    { return juce::Colour(0xFF0A0A12); }
    inline juce::Colour panelBg()       { return juce::Colour(0xFF14141C); }
    inline juce::Colour panelBorder()   { return juce::Colour(0xFF2A2A3A); }
    inline juce::Colour cyan()          { return juce::Colour(0xFF00E5FF); }
    inline juce::Colour blue()          { return juce::Colour(0xFF4060FF); }
    inline juce::Colour purple()        { return juce::Colour(0xFF8040FF); }
    inline juce::Colour green()         { return juce::Colour(0xFF40FF80); }
    inline juce::Colour orange()        { return juce::Colour(0xFFFF6020); }
    inline juce::Colour pink()          { return juce::Colour(0xFFFF2080); }
    inline juce::Colour textBright()    { return juce::Colour(0xFFE0E0F0); }
    inline juce::Colour textDim()       { return juce::Colour(0xFF808098); }
    inline juce::Colour ledOn()         { return juce::Colour(0xFF00FF80); }
    inline juce::Colour ledOff()        { return juce::Colour(0xFF1A3020); }
    inline juce::Colour knobBody()      { return juce::Colour(0xFF202030); }
    inline juce::Colour knobTrack()     { return juce::Colour(0xFF3A3A50); }
}

class TB303LookAndFeel : public juce::LookAndFeel_V4
{
public:
    TB303LookAndFeel();
    ~TB303LookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool isHighlighted, bool isDown) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool isHighlighted, bool isDown) override;

    static void paintSectionPanel(juce::Graphics& g, juce::Rectangle<int> bounds,
                                  const juce::String& title, juce::Colour accentColor);

    static void paintLED(juce::Graphics& g, float x, float y, float sz, bool on);
};
