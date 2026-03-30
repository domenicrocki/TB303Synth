#pragma once
#include <JuceHeader.h>

class KnobComponent : public juce::Component
{
public:
    KnobComponent(const juce::String& labelText, const juce::String& paramId,
                  juce::AudioProcessorValueTreeState& apvts);
    ~KnobComponent() override;

    void resized() override;
    void setLabelBelow(bool below) { labelBelow_ = below; resized(); }
    void setArcColor(juce::Colour c);

    juce::Slider& getSlider() { return slider_; }

private:
    juce::Slider slider_;
    juce::Label label_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment_;
    bool labelBelow_ = false;
};
