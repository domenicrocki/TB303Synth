#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/TB303LookAndFeel.h"
#include "GUI/TopPanel.h"
#include "GUI/MiddlePanel.h"
#include "GUI/BottomPanel.h"

class TB303AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TB303AudioProcessorEditor(TB303AudioProcessor&);
    ~TB303AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TB303AudioProcessor& processor_;
    TB303LookAndFeel lookAndFeel_;

    TopPanel topPanel_;
    MiddlePanel middlePanel_;
    BottomPanel bottomPanel_;

    // Original TB-303 proportions: wide, ~3:1 aspect ratio
    static constexpr int EDITOR_WIDTH = 1200;
    static constexpr int EDITOR_HEIGHT = 560;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TB303AudioProcessorEditor)
};
