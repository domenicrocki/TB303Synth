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
    float fc = cutoff_ / static_cast<float>(sampleRate_);
    cutoffMapped_ = fc * juce::MathConstants<float>::pi;
    // Resonance mapped to feedback gain (0 to ~4 for self-oscillation)
    resMapped_ = resonance_ * 4.0f;
}

float TB303Filter::process(float input)
{
    // Simplified diode ladder filter model
    // 4 cascaded 1-pole lowpass sections with nonlinear feedback
    float cutoffG = std::tan(cutoffMapped_);
    float feedback = resMapped_ * (1.0f / (1.0f + cutoffG));

    // Feedback from output
    float feedbackSignal = delay_[3];
    float inputWithFeedback = input - feedback * std::tanh(feedbackSignal);

    // 4 cascaded 1-pole filters
    for (int i = 0; i < 4; ++i)
    {
        float inputSample = (i == 0) ? inputWithFeedback : stage_[i - 1];
        float v = cutoffG * (std::tanh(inputSample) - std::tanh(delay_[i]));
        stage_[i] = v + delay_[i];
        delay_[i] = stage_[i] + v;
    }

    // Soft clip the output for saturation character
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
