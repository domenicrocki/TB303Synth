#pragma once
#include <JuceHeader.h>

class DelayEffect
{
public:
    enum class DelayType { Digital, Tape, PingPong };

    DelayEffect();

    void prepare(double sampleRate, int maxBlockSize);
    void setDelayType(DelayType type);
    void setTime(float timeMs);
    void setLevel(float level);
    void setFeedback(float feedback);
    void setTempoSync(bool enabled);
    void setBPM(double bpm);
    void process(float* leftChannel, float* rightChannel, int numSamples);
    void reset();

private:
    double sampleRate_ = 44100.0;
    DelayType delayType_ = DelayType::Digital;
    float timeMs_ = 250.0f;
    float level_ = 0.0f;
    float feedback_ = 0.3f;
    bool tempoSync_ = false;
    double bpm_ = 120.0;

    std::vector<float> bufferL_;
    std::vector<float> bufferR_;
    int writePos_ = 0;
    int bufferSize_ = 0;

    // Tape-style lowpass for feedback
    float tapeLPState_ = 0.0f;
    float tapeLPStateR_ = 0.0f;

    int getDelaySamples() const;
};
