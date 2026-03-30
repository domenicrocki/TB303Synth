#pragma once
#include <JuceHeader.h>

struct PatchPreset
{
    juce::String name;
    float cutoff = 1000.0f;
    float resonance = 0.3f;
    float envMod = 0.5f;
    float decay = 200.0f;
    float accent = 0.5f;
    float tuning = 0.0f;
    int waveform = 0; // 0=saw, 1=square
    float driveDepth = 0.0f;
    int driveType = 0;
    float driveTone = 0.5f;
    float delayTime = 250.0f;
    float delayLevel = 0.0f;
    float delayFeedback = 0.3f;
    int delayType = 0;
    bool tempoSync = false;
};

class PresetManager
{
public:
    PresetManager();

    int getNumPatches() const { return static_cast<int>(patches_.size()); }
    const PatchPreset& getPatch(int index) const;
    void addPatch(const PatchPreset& patch);
    void replacePatch(int index, const PatchPreset& patch);

    juce::StringArray getPatchNames() const;

    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& tree);

private:
    std::vector<PatchPreset> patches_;
    void initFactoryPresets();
};
