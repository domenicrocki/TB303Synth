#include "PresetManager.h"

PresetManager::PresetManager()
{
    initFactoryPresets();
}

void PresetManager::initFactoryPresets()
{
    patches_.clear();

    // Preset 1: Classic Acid Saw
    patches_.push_back({ "Acid Saw", 800.0f, 0.7f, 0.8f, 300.0f, 0.6f, 0.0f, 0,
                         0.0f, 0, 0.5f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 2: Acid WideOD Saw
    patches_.push_back({ "Acid WideOD Saw", 600.0f, 0.8f, 0.9f, 250.0f, 0.7f, 0.0f, 0,
                         0.6f, 0, 0.6f, 375.0f, 0.2f, 0.4f, 0, false });

    // Preset 3: Squelch Square
    patches_.push_back({ "Squelch Square", 500.0f, 0.85f, 0.95f, 200.0f, 0.5f, 0.0f, 1,
                         0.0f, 0, 0.5f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 4: Rubber Bass
    patches_.push_back({ "Rubber Bass", 400.0f, 0.4f, 0.3f, 500.0f, 0.3f, 0.0f, 0,
                         0.0f, 0, 0.5f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 5: Hard Acid
    patches_.push_back({ "Hard Acid", 700.0f, 0.9f, 1.0f, 150.0f, 0.8f, 0.0f, 0,
                         0.8f, 1, 0.4f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 6: Mellow Square
    patches_.push_back({ "Mellow Square", 1200.0f, 0.2f, 0.2f, 800.0f, 0.2f, 0.0f, 1,
                         0.0f, 0, 0.5f, 500.0f, 0.15f, 0.3f, 0, false });

    // Preset 7: Screaming Lead
    patches_.push_back({ "Screaming Lead", 2000.0f, 0.95f, 0.7f, 100.0f, 0.9f, 7.0f, 0,
                         0.3f, 2, 0.7f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 8: Deep Sub
    patches_.push_back({ "Deep Sub", 300.0f, 0.3f, 0.1f, 1000.0f, 0.2f, -5.0f, 0,
                         0.0f, 0, 0.5f, 250.0f, 0.0f, 0.3f, 0, false });

    // Preset 9: Delay Acid
    patches_.push_back({ "Delay Acid", 600.0f, 0.75f, 0.85f, 200.0f, 0.6f, 0.0f, 0,
                         0.4f, 0, 0.5f, 375.0f, 0.4f, 0.5f, 1, true });

    // Preset 10: Tube Distortion
    patches_.push_back({ "Tube Distortion", 900.0f, 0.6f, 0.7f, 350.0f, 0.5f, 0.0f, 0,
                         0.9f, 2, 0.3f, 250.0f, 0.0f, 0.3f, 0, false });
}

const PatchPreset& PresetManager::getPatch(int index) const
{
    return patches_[static_cast<size_t>(juce::jlimit(0, static_cast<int>(patches_.size()) - 1, index))];
}

void PresetManager::addPatch(const PatchPreset& patch)
{
    patches_.push_back(patch);
}

void PresetManager::replacePatch(int index, const PatchPreset& patch)
{
    if (index >= 0 && index < static_cast<int>(patches_.size()))
        patches_[static_cast<size_t>(index)] = patch;
}

juce::StringArray PresetManager::getPatchNames() const
{
    juce::StringArray names;
    for (auto& p : patches_)
        names.add(p.name);
    return names;
}

juce::ValueTree PresetManager::toValueTree() const
{
    juce::ValueTree tree("Presets");
    for (auto& p : patches_)
    {
        juce::ValueTree pt("Patch");
        pt.setProperty("name", p.name, nullptr);
        pt.setProperty("cutoff", p.cutoff, nullptr);
        pt.setProperty("resonance", p.resonance, nullptr);
        pt.setProperty("envMod", p.envMod, nullptr);
        pt.setProperty("decay", p.decay, nullptr);
        pt.setProperty("accent", p.accent, nullptr);
        pt.setProperty("tuning", p.tuning, nullptr);
        pt.setProperty("waveform", p.waveform, nullptr);
        pt.setProperty("driveDepth", p.driveDepth, nullptr);
        pt.setProperty("driveType", p.driveType, nullptr);
        pt.setProperty("driveTone", p.driveTone, nullptr);
        pt.setProperty("delayTime", p.delayTime, nullptr);
        pt.setProperty("delayLevel", p.delayLevel, nullptr);
        pt.setProperty("delayFeedback", p.delayFeedback, nullptr);
        pt.setProperty("delayType", p.delayType, nullptr);
        pt.setProperty("tempoSync", p.tempoSync, nullptr);
        tree.addChild(pt, -1, nullptr);
    }
    return tree;
}

void PresetManager::fromValueTree(const juce::ValueTree& tree)
{
    patches_.clear();
    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto pt = tree.getChild(i);
        PatchPreset p;
        p.name = pt.getProperty("name", "Init");
        p.cutoff = pt.getProperty("cutoff", 1000.0f);
        p.resonance = pt.getProperty("resonance", 0.3f);
        p.envMod = pt.getProperty("envMod", 0.5f);
        p.decay = pt.getProperty("decay", 200.0f);
        p.accent = pt.getProperty("accent", 0.5f);
        p.tuning = pt.getProperty("tuning", 0.0f);
        p.waveform = pt.getProperty("waveform", 0);
        p.driveDepth = pt.getProperty("driveDepth", 0.0f);
        p.driveType = pt.getProperty("driveType", 0);
        p.driveTone = pt.getProperty("driveTone", 0.5f);
        p.delayTime = pt.getProperty("delayTime", 250.0f);
        p.delayLevel = pt.getProperty("delayLevel", 0.0f);
        p.delayFeedback = pt.getProperty("delayFeedback", 0.3f);
        p.delayType = pt.getProperty("delayType", 0);
        p.tempoSync = pt.getProperty("tempoSync", false);
        patches_.push_back(p);
    }
    if (patches_.empty())
        initFactoryPresets();
}
