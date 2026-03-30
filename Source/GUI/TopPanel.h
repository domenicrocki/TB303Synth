#pragma once
#include <JuceHeader.h>
#include "KnobComponent.h"

class TopPanel : public juce::Component
{
public:
    TopPanel(juce::AudioProcessorValueTreeState& apvts);
    ~TopPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Waveform toggle
    juce::TextButton waveformButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> waveformAttachment_;

    // Main synth knobs
    KnobComponent tuningKnob_;
    KnobComponent cutoffKnob_;
    KnobComponent resonanceKnob_;
    KnobComponent envModKnob_;
    KnobComponent decayKnob_;
    KnobComponent accentKnob_;
    KnobComponent masterTuneKnob_;

    // Drive section
    juce::ComboBox driveTypeBox_;
    KnobComponent driveToneKnob_;
    KnobComponent driveDepthKnob_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> driveTypeAttachment_;

    // Delay section
    juce::ComboBox delayTypeBox_;
    KnobComponent delayTimeKnob_;
    KnobComponent delayLevelKnob_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> delayTypeAttachment_;

    // Tempo sync
    juce::ToggleButton tempoSyncButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncAttachment_;

    // Volume
    KnobComponent volumeKnob_;

    juce::AudioProcessorValueTreeState& apvts_;

    int currentWaveform_ = 0;
};
