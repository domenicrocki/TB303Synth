#pragma once
#include <JuceHeader.h>

class LEDButton : public juce::Component
{
public:
    LEDButton(const juce::String& text);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

    void setLEDOn(bool on);
    bool isLEDOn() const { return ledOn_; }
    void setToggleMode(bool toggle) { toggleMode_ = toggle; }
    void setButtonText(const juce::String& text) { buttonText_ = text; repaint(); }
    void setAccentColor(juce::Colour c) { accentColor_ = c; repaint(); }

    std::function<void(bool)> onClick;

private:
    juce::String buttonText_;
    bool ledOn_ = false;
    bool toggleMode_ = true;
    juce::Colour accentColor_ { 0xFF00E5FF };
    juce::Rectangle<float> buttonBounds_;
};
