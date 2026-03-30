#pragma once
#include <JuceHeader.h>

class TB303Filter
{
public:
    TB303Filter();

    void prepare(double sampleRate);
    void setCutoff(float cutoffHz);
    void setResonance(float resonance);
    float process(float input);
    void reset();

private:
    void updateCoefficients();

    double sampleRate_ = 44100.0;
    float cutoff_ = 1000.0f;
    float resonance_ = 0.0f;

    // 4-pole diode ladder filter state
    float stage_[4] = {};
    float delay_[4] = {};
    float tanhStage_[3] = {};

    float cutoffMapped_ = 0.0f;
    float resMapped_ = 0.0f;
};
