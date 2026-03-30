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
    juce::AudioProcessorValueTreeState& apvts_;

    // Waveform toggle
    juce::TextButton waveformButton_;
    int currentWaveform_ = 0;

    // Main synth knobs
    KnobComponent tuningKnob_;
    KnobComponent cutoffKnob_;
    KnobComponent resonanceKnob_;
    KnobComponent envModKnob_;
    KnobComponent decayKnob_;
    KnobComponent accentKnob_;

    // Master section
    KnobComponent masterTuneKnob_;

    // VCF Trim and Condition (cosmetic small knobs)
    juce::Slider vcfTrimSlider_;
    juce::Slider conditionSlider_;

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

    // Volume (large knob - custom painted)
    juce::Slider volumeSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment_;
};
