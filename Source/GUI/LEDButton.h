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

    std::function<void(bool)> onClick;

private:
    juce::String buttonText_;
    bool ledOn_ = false;
    bool toggleMode_ = true;
    juce::Rectangle<float> buttonBounds_;
    juce::Rectangle<float> ledBounds_;
};
