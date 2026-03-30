#include "TB303Envelope.h"

TB303Envelope::TB303Envelope() {}

void TB303Envelope::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    float attackSamples = (ATTACK_TIME_MS / 1000.0f) * static_cast<float>(sampleRate_);
    attackRate_ = 1.0f / std::max(1.0f, attackSamples);

    float releaseSamples = (RELEASE_TIME_MS / 1000.0f) * static_cast<float>(sampleRate_);
    releaseRate_ = std::exp(-1.0f / std::max(1.0f, releaseSamples));

    setDecay(decayMs_);
}

void TB303Envelope::setDecay(float decayMs)
{
    decayMs_ = juce::jlimit(30.0f, 3000.0f, decayMs);
    float decaySamples = (decayMs_ / 1000.0f) * static_cast<float>(sampleRate_);
    decayRate_ = std::exp(-1.0f / std::max(1.0f, decaySamples));
}

void TB303Envelope::setSustainLevel(float level)
{
    sustainLevel_ = juce::jlimit(0.0f, 1.0f, level);
}

void TB303Envelope::trigger()
{
    state_ = State::Attack;
}

void TB303Envelope::release()
{
    if (state_ != State::Idle)
        state_ = State::Idle; // Fast release for 303
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
        {
            output_ = sustainLevel_ + (output_ - sustainLevel_) * decayRate_;
            float threshold = sustainLevel_ + 0.001f;
            if (output_ <= threshold)
            {
                output_ = sustainLevel_;
                state_ = (sustainLevel_ > 0.0f) ? State::Sustain : State::Idle;
            }
            break;
        }

        case State::Sustain:
            output_ = sustainLevel_;
            break;

        case State::Idle:
            // Quick release ramp
            if (output_ > 0.0001f)
                output_ *= releaseRate_;
            else
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
