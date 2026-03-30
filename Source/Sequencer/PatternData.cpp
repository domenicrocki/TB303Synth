#include "PatternData.h"

PatternData::PatternData()
{
    initDefaults();
}

Pattern& PatternData::getPattern(int bank, int pattern)
{
    return patterns_[juce::jlimit(0, NUM_BANKS - 1, bank)]
                    [juce::jlimit(0, PATTERNS_PER_BANK - 1, pattern)];
}

const Pattern& PatternData::getPattern(int bank, int pattern) const
{
    return patterns_[juce::jlimit(0, NUM_BANKS - 1, bank)]
                    [juce::jlimit(0, PATTERNS_PER_BANK - 1, pattern)];
}

void PatternData::initDefaults()
{
    // Classic acid pattern in bank 0, pattern 0
    {
        auto& p = patterns_[0][0];
        const int notes[] = { 36, 36, 39, 36, 36, 41, 36, 43,
                              36, 36, 39, 36, 41, 43, 41, 39 };
        for (int i = 0; i < 16; ++i)
        {
            p.steps[i].note = notes[i];
            p.steps[i].octave = 0;
            p.steps[i].active = true;
            p.steps[i].accent = (i % 4 == 0);
            p.steps[i].slide = (i == 2 || i == 6 || i == 10 || i == 14);
        }
    }

    // Second pattern - simple bassline
    {
        auto& p = patterns_[0][1];
        const int notes[] = { 36, 36, 36, 48, 36, 36, 48, 36,
                              36, 36, 36, 48, 36, 48, 36, 36 };
        for (int i = 0; i < 16; ++i)
        {
            p.steps[i].note = notes[i];
            p.steps[i].octave = 0;
            p.steps[i].active = (i != 3 && i != 11);
            p.steps[i].accent = (i == 0 || i == 4 || i == 8 || i == 12);
            p.steps[i].slide = false;
        }
    }

    // Third pattern - acid squelch
    {
        auto& p = patterns_[0][2];
        const int notes[] = { 48, 51, 48, 46, 48, 53, 51, 48,
                              46, 48, 51, 53, 48, 46, 48, 51 };
        for (int i = 0; i < 16; ++i)
        {
            p.steps[i].note = notes[i];
            p.steps[i].octave = 0;
            p.steps[i].active = true;
            p.steps[i].accent = (i % 3 == 0);
            p.steps[i].slide = (i % 2 == 1);
        }
    }

    // Remaining patterns start as simple C notes
    for (int bank = 0; bank < NUM_BANKS; ++bank)
    {
        int startPat = (bank == 0) ? 3 : 0;
        for (int pat = startPat; pat < PATTERNS_PER_BANK; ++pat)
        {
            auto& p = patterns_[bank][pat];
            for (int i = 0; i < 16; ++i)
            {
                p.steps[i].note = 36;
                p.steps[i].octave = 0;
                p.steps[i].active = (i % 4 == 0);
                p.steps[i].accent = false;
                p.steps[i].slide = false;
            }
        }
    }
}

juce::ValueTree PatternData::toValueTree() const
{
    juce::ValueTree tree("PatternData");
    for (int b = 0; b < NUM_BANKS; ++b)
    {
        juce::ValueTree bankTree("Bank");
        bankTree.setProperty("index", b, nullptr);
        for (int p = 0; p < PATTERNS_PER_BANK; ++p)
        {
            bankTree.addChild(patterns_[b][p].toValueTree(), p, nullptr);
        }
        tree.addChild(bankTree, b, nullptr);
    }
    return tree;
}

void PatternData::fromValueTree(const juce::ValueTree& tree)
{
    for (int b = 0; b < NUM_BANKS && b < tree.getNumChildren(); ++b)
    {
        auto bankTree = tree.getChild(b);
        for (int p = 0; p < PATTERNS_PER_BANK && p < bankTree.getNumChildren(); ++p)
        {
            patterns_[b][p].fromValueTree(bankTree.getChild(p));
        }
    }
}
