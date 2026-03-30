#include "TB303Envelope.h"

TB303Envelope::TB303Envelope() {}

void TB303Envelope::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    float attackSamples = (ATTACK_TIME_MS / 1000.0f) * static_cast<float>(sampleRate_);
    attackRate_ = 1.0f / std::max(1.0f, attackSamples);
    setDecay(decayMs_);
}

void TB303Envelope::setDecay(float decayMs)
{
    decayMs_ = juce::jlimit(30.0f, 3000.0f, decayMs);
    float decaySamples = (decayMs_ / 1000.0f) * static_cast<float>(sampleRate_);
    // Exponential decay coefficient
    decayRate_ = std::exp(-1.0f / std::max(1.0f, decaySamples));
}

void TB303Envelope::trigger()
{
    state_ = State::Attack;
}

void TB303Envelope::release()
{
    if (state_ != State::Idle)
        state_ = State::Decay;
}

float TB303Envelope::process()
{
    switch (state_)
    {
        case State::Attack:
            output_ += attackRate_;
            if (output_ >= 1.0f)
            {
                output_ = 1.0f;
                state_ = State::Decay;
            }
            break;

        case State::Decay:
            output_ *= decayRate_;
            if (output_ < 0.001f)
            {
                output_ = 0.0f;
                state_ = State::Idle;
            }
            break;

        case State::Idle:
            output_ = 0.0f;
            break;
    }

    return output_;
}

void TB303Envelope::reset()
{
    state_ = State::Idle;
    output_ = 0.0f;
}
