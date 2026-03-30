#pragma once
#include <JuceHeader.h>

class TB303Envelope
{
public:
    enum class State { Idle, Attack, Decay, Sustain };

    TB303Envelope();

    void prepare(double sampleRate);
    void setDecay(float decayMs);
    void setSustainLevel(float level);
    void trigger();
    void release();
    float process();
    void reset();
    bool isActive() const { return state_ != State::Idle; }

private:
    double sampleRate_ = 44100.0;
    State state_ = State::Idle;
    float output_ = 0.0f;
    float attackRate_ = 0.0f;
    float decayRate_ = 0.0f;
    float sustainLevel_ = 0.0f;
    float decayMs_ = 200.0f;
    float releaseRate_ = 0.0f;

    static constexpr float ATTACK_TIME_MS = 3.0f;
    static constexpr float RELEASE_TIME_MS = 10.0f;
};
