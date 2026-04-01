#include "TB303Filter.h"

TB303Filter::TB303Filter() {}

void TB303Filter::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    reset();
}

void TB303Filter::setCutoff(float cutoffHz)
{
    cutoff_ = juce::jlimit(20.0f, 20000.0f, cutoffHz);
    updateCoefficients();
}

void TB303Filter::setResonance(float resonance)
{
    resonance_ = juce::jlimit(0.0f, 1.0f, resonance);
    updateCoefficients();
}

void TB303Filter::updateCoefficients()
{
    // Pre-warp cutoff frequency
    float fc = cutoff_ / static_cast<float>(sampleRate_);
    fc = juce::jlimit(0.0001f, 0.49f, fc);
    cutoffMapped_ = std::tan(juce::MathConstants<float>::pi * fc);

    // Resonance: 0-1 mapped to 0-3.5 feedback (controlled self-oscillation)
    resMapped_ = resonance_ * 3.5f;
}

float TB303Filter::process(float input)
{
    // Improved diode ladder filter with stable feedback
    float g = cutoffMapped_;
    float G = g / (1.0f + g); // one-pole gain coefficient

    // Feedback with compensation to prevent blowup
    float feedbackSignal = delay_[3];
    float feedback = resMapped_ * (1.0f / (1.0f + resMapped_ * 0.25f));
    float inputWithFeedback = input - feedback * std::tanh(feedbackSignal);

    // Limit input to prevent runaway
    inputWithFeedback = juce::jlimit(-5.0f, 5.0f, inputWithFeedback);

    // 4 cascaded 1-pole filters (trapezoidal integration)
    for (int i = 0; i < 4; ++i)
    {
        float inputSample = (i == 0) ? std::tanh(inputWithFeedback) : stage_[i - 1];
        float v = G * (inputSample - delay_[i]);
        stage_[i] = v + delay_[i];
        delay_[i] = stage_[i] + v;

        // Soft clip each stage to prevent accumulation
        delay_[i] = juce::jlimit(-4.0f, 4.0f, delay_[i]);
    }

    // Soft clip output
    return std::tanh(stage_[3]);
}

void TB303Filter::reset()
{
    for (int i = 0; i < 4; ++i)
    {
        stage_[i] = 0.0f;
        delay_[i] = 0.0f;
    }
    updateCoefficients();
}
