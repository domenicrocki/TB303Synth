#include "DriveEffect.h"

DriveEffect::DriveEffect() {}

void DriveEffect::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    toneFilterState_ = 0.0f;
    updateToneFilter();
}

void DriveEffect::setDriveType(DriveType type)
{
    driveType_ = type;
}

void DriveEffect::setDepth(float depth)
{
    depth_ = juce::jlimit(0.0f, 1.0f, depth);
}

void DriveEffect::setTone(float tone)
{
    tone_ = juce::jlimit(0.0f, 1.0f, tone);
    updateToneFilter();
}

void DriveEffect::updateToneFilter()
{
    // Map tone 0-1 to filter coefficient (dark to bright)
    float freq = 500.0f + tone_ * 15000.0f;
    float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * freq);
    float dt = 1.0f / static_cast<float>(sampleRate_);
    toneCoeff_ = dt / (rc + dt);
}

float DriveEffect::process(float input)
{
    if (depth_ < 0.001f)
        return input;

    float gain = 1.0f + depth_ * 20.0f;
    float driven = input * gain;

    switch (driveType_)
    {
        case DriveType::Soft:
            driven = std::tanh(driven);
            break;

        case DriveType::Hard:
            driven = juce::jlimit(-1.0f, 1.0f, driven);
            break;

        case DriveType::Tube:
        {
            // Asymmetric soft clip for tube character
            if (driven > 0.0f)
                driven = 1.0f - std::exp(-driven);
            else
                driven = -1.0f + std::exp(driven);
            break;
        }
    }

    // Apply tone filter
    toneFilterState_ += toneCoeff_ * (driven - toneFilterState_);
    float toned = toneFilterState_ + (driven - toneFilterState_) * tone_;

    // Mix: blend between dry and driven based on depth
    return input * (1.0f - depth_) + toned * depth_;
}

void DriveEffect::reset()
{
    toneFilterState_ = 0.0f;
}
