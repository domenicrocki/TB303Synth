#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"

class MiddlePanel : public juce::Component
{
public:
    MiddlePanel(TB303AudioProcessor& processor);
    ~MiddlePanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiddlePanel)
};
