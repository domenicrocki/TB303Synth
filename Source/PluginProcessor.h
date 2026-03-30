#pragma once
#include <JuceHeader.h>
#include "DSP/TB303Voice.h"
#include "DSP/DriveEffect.h"
#include "DSP/DelayEffect.h"
#include "Sequencer/StepSequencer.h"
#include "Presets/PresetManager.h"

class TB303AudioProcessor : public juce::AudioProcessor
{
public:
    TB303AudioProcessor();
    ~TB303AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    StepSequencer& getSequencer() { return sequencer_; }
    PresetManager& getPresetManager() { return presetManager_; }

    void loadPatch(int index);

private:
    juce::AudioProcessorValueTreeState apvts_;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    TB303Voice voice_;
    DriveEffect drive_;
    DelayEffect delay_;
    StepSequencer sequencer_;
    PresetManager presetManager_;

    std::atomic<float>* waveformParam = nullptr;
    std::atomic<float>* tuningParam = nullptr;
    std::atomic<float>* cutoffParam = nullptr;
    std::atomic<float>* resonanceParam = nullptr;
    std::atomic<float>* envModParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* accentParam = nullptr;
    std::atomic<float>* volumeParam = nullptr;
    std::atomic<float>* masterTuneParam = nullptr;
    std::atomic<float>* driveTypeParam = nullptr;
    std::atomic<float>* driveToneParam = nullptr;
    std::atomic<float>* driveDepthParam = nullptr;
    std::atomic<float>* delayTypeParam = nullptr;
    std::atomic<float>* delayTimeParam = nullptr;
    std::atomic<float>* delayLevelParam = nullptr;
    std::atomic<float>* delayFeedbackParam = nullptr;
    std::atomic<float>* tempoSyncParam = nullptr;
    std::atomic<float>* shuffleParam = nullptr;
    std::atomic<float>* scaleParam = nullptr;
    std::atomic<float>* playModeParam = nullptr;
    std::atomic<float>* tempoParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TB303AudioProcessor)
};
