#pragma once
#include <JuceHeader.h>
#include "LEDButton.h"
#include "StepButton.h"
#include "PianoKeyboard.h"
#include "../PluginProcessor.h"

class BottomPanel : public juce::Component, public juce::Timer
{
public:
    BottomPanel(TB303AudioProcessor& processor);
    ~BottomPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void updateStepDisplay();

private:
    TB303AudioProcessor& processor_;

    // Left column
    juce::ToggleButton posLockButton_{ "POSITION LOCK\nTO DAW" };
    LEDButton runStopButton_{ "RUN / STOP" };

    // Keyboard section
    juce::ToggleButton keyboardToggle_{ "KEYBOARD" };
    LEDButton editButton_{ "EDIT" };
    PianoKeyboard keyboard_;

    // Right side buttons (top row)
    LEDButton accentButton_{ "ACCENT" };
    LEDButton slideButton_{ "SLIDE" };
    LEDButton downButton_{ "DOWN" };
    LEDButton upButton_{ "UP" };

    // Far right
    juce::TextButton randomizeButton_{ "RANDOMIZE" };
    juce::TextButton generateButton_{ "GENERATE /\nUNDO" };

    // Bottom row
    juce::OwnedArray<StepButton> stepButtons_;
    juce::TextButton modifyButton_{ "MODIFY /\nUNDO" };

    bool editMode_ = false;
    int editStep_ = 0;
    Pattern undoPattern_;
    bool hasUndo_ = false;
};
