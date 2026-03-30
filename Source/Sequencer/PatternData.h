#pragma once
#include <JuceHeader.h>

struct Step
{
    int note = 60;       // MIDI note (C4 = 60)
    int octave = 0;      // Octave offset (-1, 0, +1)
    bool accent = false;
    bool slide = false;
    bool active = true;   // Note on/off (rest if false)
};

struct Pattern
{
    Step steps[16];
    int numSteps = 16;

    void randomize(juce::Random& rng)
    {
        for (int i = 0; i < 16; ++i)
        {
            steps[i].note = 36 + rng.nextInt(25);  // C2 to C4
            steps[i].octave = rng.nextInt(3) - 1;
            steps[i].accent = rng.nextFloat() < 0.25f;
            steps[i].slide = rng.nextFloat() < 0.2f;
            steps[i].active = rng.nextFloat() < 0.75f;
        }
    }

    juce::ValueTree toValueTree() const
    {
        juce::ValueTree tree("Pattern");
        tree.setProperty("numSteps", numSteps, nullptr);
        for (int i = 0; i < 16; ++i)
        {
            juce::ValueTree stepTree("Step");
            stepTree.setProperty("note", steps[i].note, nullptr);
            stepTree.setProperty("octave", steps[i].octave, nullptr);
            stepTree.setProperty("accent", steps[i].accent, nullptr);
            stepTree.setProperty("slide", steps[i].slide, nullptr);
            stepTree.setProperty("active", steps[i].active, nullptr);
            tree.addChild(stepTree, i, nullptr);
        }
        return tree;
    }

    void fromValueTree(const juce::ValueTree& tree)
    {
        numSteps = tree.getProperty("numSteps", 16);
        for (int i = 0; i < 16 && i < tree.getNumChildren(); ++i)
        {
            auto stepTree = tree.getChild(i);
            steps[i].note = stepTree.getProperty("note", 60);
            steps[i].octave = stepTree.getProperty("octave", 0);
            steps[i].accent = stepTree.getProperty("accent", false);
            steps[i].slide = stepTree.getProperty("slide", false);
            steps[i].active = stepTree.getProperty("active", true);
        }
    }
};

class PatternData
{
public:
    static constexpr int NUM_BANKS = 8;
    static constexpr int PATTERNS_PER_BANK = 8;

    PatternData();

    Pattern& getPattern(int bank, int pattern);
    const Pattern& getPattern(int bank, int pattern) const;

    void initDefaults();
    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& tree);

private:
    Pattern patterns_[NUM_BANKS][PATTERNS_PER_BANK];
};
