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

    juce::ToggleButton posLockButton_{ "POSITION LOCK\nTO DAW" };
    juce::ToggleButton keyboardToggle_{ "KEYBOARD" };

    PianoKeyboard keyboard_;

    LEDButton runStopButton_{ "RUN / STOP" };
    LEDButton editButton_{ "EDIT" };
    LEDButton accentButton_{ "ACCENT" };
    LEDButton slideButton_{ "SLIDE" };
    LEDButton downButton_{ "DOWN" };
    LEDButton upButton_{ "UP" };

    juce::OwnedArray<StepButton> stepButtons_;

    juce::TextButton randomizeButton_{ "RANDOMIZE" };
    juce::TextButton modifyButton_{ "MODIFY/\nUNDO" };

    bool editMode_ = false;
    int editStep_ = 0;
    Pattern undoPattern_;
    bool hasUndo_ = false;
};
