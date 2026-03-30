#pragma once
#include <JuceHeader.h>

class StepButton : public juce::Component
{
public:
    StepButton(int stepIndex);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    void setActive(bool active);
    void setCurrent(bool current);
    bool isActive() const { return active_; }
    int getStepIndex() const { return stepIndex_; }

    std::function<void(int, bool)> onToggle;

private:
    int stepIndex_;
    bool active_ = false;
    bool current_ = false;
};
