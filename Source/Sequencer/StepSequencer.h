#pragma once
#include <JuceHeader.h>
#include "PatternData.h"
#include "../DSP/TB303Voice.h"

class StepSequencer
{
public:
    enum class PlayMode { Forward, Reverse, FwdAndRev, Invert, Random };
    enum class Scale { Sixteenth, SixteenthTriplet, ThirtySecond };

    StepSequencer();

    void prepare(double sampleRate);
    void processBlock(int numSamples, TB303Voice& voice);
    void start();
    void stop();
    bool isPlaying() const { return playing_; }

    // Parameters
    void setTempo(double bpm);
    void setPlayMode(PlayMode mode);
    void setScale(Scale scale);
    void setShuffle(float amount);
    void setCurrentBank(int bank);
    void setCurrentPattern(int pattern);
    int getCurrentBank() const { return currentBank_; }
    int getCurrentPattern() const { return currentPattern_; }
    int getCurrentStep() const { return currentStep_; }

    void randomizeCurrentPattern();
    void setHostSync(bool enabled);
    void setHostPosition(double ppqPosition, double bpm, bool isPlaying);

    PatternData& getPatternData() { return patternData_; }
    const PatternData& getPatternData() const { return patternData_; }

    Pattern& getCurrentPatternRef();

private:
    int getNextStep();

    PatternData patternData_;
    double sampleRate_ = 44100.0;
    double bpm_ = 120.0;
    double samplesPerStep_ = 0.0;
    double sampleCounter_ = 0.0;

    int currentStep_ = 0;
    int currentBank_ = 0;
    int currentPattern_ = 0;
    bool playing_ = false;
    bool forward_ = true; // for FWD&REV mode

    PlayMode playMode_ = PlayMode::Forward;
    Scale scale_ = Scale::Sixteenth;
    float shuffle_ = 0.0f;

    bool hostSync_ = false;
    double hostPPQ_ = 0.0;
    bool hostPlaying_ = false;

    juce::Random random_;

    void updateTiming();
};
