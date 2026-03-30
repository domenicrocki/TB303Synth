#pragma once
#include <JuceHeader.h>
#include "KnobComponent.h"
#include "../PluginProcessor.h"

class MiddlePanel : public juce::Component
{
public:
    MiddlePanel(TB303AudioProcessor& processor);
    ~MiddlePanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void updatePatchList();

private:
    TB303AudioProcessor& processor_;

    juce::TextButton optionButton_{ "OPTION" };
    juce::TextButton helpButton_{ "HELP" };
    juce::TextButton aboutButton_{ "ABOUT" };

    // Bank buttons
    juce::OwnedArray<juce::TextButton> bankButtons_;

    KnobComponent shuffleKnob_;
    KnobComponent scaleKnob_;

    // Play mode - rotary selector
    juce::ComboBox playModeBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> playModeAttachment_;

    // Pattern list
    juce::Label patternLabel_{ {}, "PATTERN" };
    juce::ComboBox patternList_;
    juce::TextButton patternUp_{ ">" };
    juce::TextButton patternDown_{ "<" };
    juce::TextButton patternWrite_{ "WRITE" };

    // Patch list
    juce::Label patchLabel_{ {}, "PATCH" };
    juce::ComboBox patchList_;
    juce::TextButton patchUp_{ ">" };
    juce::TextButton patchDown_{ "<" };
    juce::TextButton patchWrite_{ "WRITE" };

    // Tempo knob
    KnobComponent tempoKnob_;
};
