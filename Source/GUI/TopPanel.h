#pragma once
#include <JuceHeader.h>
#include "KnobComponent.h"
#include "../PluginProcessor.h"

class TopPanel : public juce::Component, public juce::Timer
{
public:
    TopPanel(TB303AudioProcessor& processor);
    ~TopPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    TB303AudioProcessor& processor_;
    juce::AudioProcessorValueTreeState& apvts_;

    // --- Row 1 ---
    // VCO
    juce::TextButton sawButton_;
    juce::TextButton sqrButton_;
    int currentWaveform_ = 0;
    KnobComponent tuningKnob_;

    // VCF
    KnobComponent cutoffKnob_;
    KnobComponent resonanceKnob_;

    // Filter Envelope
    KnobComponent envModKnob_;
    KnobComponent decayKnob_;
    KnobComponent accentKnob_;

    // VCA
    juce::Slider volumeSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment_;

    // --- Row 2 ---
    // Clock
    KnobComponent tempoKnob_;

    // Overdrive
    juce::ComboBox driveTypeBox_;
    KnobComponent driveDepthKnob_;
    KnobComponent driveToneKnob_;
    KnobComponent driveLevelKnob_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> driveTypeAttachment_;

    // Delay
    juce::ComboBox delayTypeBox_;
    KnobComponent delayTimeKnob_;
    KnobComponent delayLevelKnob_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> delayTypeAttachment_;
    juce::ToggleButton tempoSyncButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncAttachment_;

    // Shuffle
    KnobComponent shuffleKnob_;

    // (Sequencer controls moved to BottomPanel)
};
