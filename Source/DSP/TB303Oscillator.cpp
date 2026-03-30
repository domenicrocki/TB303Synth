#include "TB303Oscillator.h"

TB303Oscillator::TB303Oscillator() {}

void TB303Oscillator::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    phase_ = 0.0f;
}

void TB303Oscillator::setFrequency(float freq, bool glide)
{
    float tunedFreq = freq * std::pow(2.0f, tuningOffset_ / 12.0f);
    targetFrequency_ = tunedFreq;

    if (glide && currentFrequency_ > 0.0f)
    {
        gliding_ = true;
        float glideTimeSamples = (GLIDE_TIME_MS / 1000.0f) * static_cast<float>(sampleRate_);
        float ratio = targetFrequency_ / currentFrequency_;
        glideRate_ = std::pow(ratio, 1.0f / glideTimeSamples);
    }
    else
    {
        gliding_ = false;
        currentFrequency_ = tunedFreq;
        phaseIncrement_ = currentFrequency_ / static_cast<float>(sampleRate_);
    }
}

void TB303Oscillator::setWaveform(Waveform wf)
{
    waveform_ = wf;
}

void TB303Oscillator::setTuning(float semitones)
{
    tuningOffset_ = semitones;
}

float TB303Oscillator::process()
{
    if (gliding_)
    {
        currentFrequency_ *= glideRate_;
        float diff = targetFrequency_ - currentFrequency_;
        if (std::abs(diff) < 0.1f)
        {
            currentFrequency_ = targetFrequency_;
            gliding_ = false;
        }
        phaseIncrement_ = currentFrequency_ / static_cast<float>(sampleRate_);
    }

    float output = 0.0f;
    if (waveform_ == Waveform::Sawtooth)
        output = polyBlepSaw(phase_);
    else
        output = polyBlepSquare(phase_);

    phase_ += phaseIncrement_;
    if (phase_ >= 1.0f)
        phase_ -= 1.0f;

    return output;
}

void TB303Oscillator::reset()
{
    phase_ = 0.0f;
    currentFrequency_ = 440.0f;
    targetFrequency_ = 440.0f;
    gliding_ = false;
}

float TB303Oscillator::polyBlepSaw(float phase)
{
    float value = 2.0f * phase - 1.0f;
    value -= polyBlep(phase);
    return value;
}

float TB303Oscillator::polyBlepSquare(float phase)
{
    float value = (phase < 0.5f) ? 1.0f : -1.0f;
    value += polyBlep(phase);
    value -= polyBlep(std::fmod(phase + 0.5f, 1.0f));
    return value;
}

float TB303Oscillator::polyBlep(float t)
{
    float dt = phaseIncrement_;
    if (dt <= 0.0f) return 0.0f;

    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if (t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}
