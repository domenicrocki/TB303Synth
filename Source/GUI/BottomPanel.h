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
    void updatePatchList();

private:
    TB303AudioProcessor& processor_;

    // Row 3 - Transport
    LEDButton runStopButton_{ "RUN / STOP" };
    LEDButton recordButton_{ "RECORD" };

    // Keyboard
    PianoKeyboard keyboard_;

    // Octave
    LEDButton downButton_{ "DOWN" };
    LEDButton upButton_{ "UP" };

    // Articulation
    LEDButton accentButton_{ "ACCENT" };
    LEDButton slideButton_{ "SLIDE" };

    // Pattern actions (under "RANDOMIZE" label)
    juce::TextButton generateUndoButton_{ "GENERATE /\nUNDO" };
    juce::TextButton modifyUndoButton_{ "MODIFY /\nUNDO" };
    juce::TextButton patternClearButton_{ "PATTERN\nCLEAR" };

    // Sequencer controls (moved from TopPanel)
    juce::ComboBox scaleBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttachment_;
    juce::ComboBox playModeBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> playModeAttachment_;
    juce::ComboBox patternList_;
    juce::ComboBox patchList_;
    juce::OwnedArray<juce::TextButton> bankButtons_;

    // Step buttons - ALL 16
    juce::OwnedArray<StepButton> stepButtons_;

    bool editMode_ = false;
    int editStep_ = 0;
    Pattern generateUndoPattern_;
    bool hasGenerateUndo_ = false;
    Pattern modifyUndoPattern_;
    bool hasModifyUndo_ = false;
};
