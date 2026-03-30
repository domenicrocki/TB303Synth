#include "DelayEffect.h"

DelayEffect::DelayEffect() {}

void DelayEffect::prepare(double sampleRate, int /*maxBlockSize*/)
{
    sampleRate_ = sampleRate;
    bufferSize_ = static_cast<int>(sampleRate * 2.0); // max 2 seconds
    bufferL_.resize(static_cast<size_t>(bufferSize_), 0.0f);
    bufferR_.resize(static_cast<size_t>(bufferSize_), 0.0f);
    writePos_ = 0;
    tapeLPState_ = 0.0f;
}

void DelayEffect::setDelayType(DelayType type) { delayType_ = type; }
void DelayEffect::setTime(float timeMs) { timeMs_ = juce::jlimit(1.0f, 2000.0f, timeMs); }
void DelayEffect::setLevel(float level) { level_ = juce::jlimit(0.0f, 1.0f, level); }
void DelayEffect::setFeedback(float feedback) { feedback_ = juce::jlimit(0.0f, 0.95f, feedback); }
void DelayEffect::setTempoSync(bool enabled) { tempoSync_ = enabled; }
void DelayEffect::setBPM(double bpm) { bpm_ = juce::jlimit(20.0, 300.0, bpm); }

int DelayEffect::getDelaySamples() const
{
    float actualTimeMs = timeMs_;

    if (tempoSync_)
    {
        // Snap to nearest musical division
        float beatMs = 60000.0f / static_cast<float>(bpm_);
        // Map timeMs range to note divisions
        if (timeMs_ < 200.0f)
            actualTimeMs = beatMs * 0.25f;      // 1/16
        else if (timeMs_ < 500.0f)
            actualTimeMs = beatMs * 0.333f;     // 1/8T
        else if (timeMs_ < 800.0f)
            actualTimeMs = beatMs * 0.5f;       // 1/8
        else if (timeMs_ < 1200.0f)
            actualTimeMs = beatMs * 0.75f;      // dotted 1/8
        else
            actualTimeMs = beatMs;              // 1/4
    }

    int samples = static_cast<int>(actualTimeMs / 1000.0f * static_cast<float>(sampleRate_));
    return juce::jlimit(1, bufferSize_ - 1, samples);
}

void DelayEffect::process(float* leftChannel, float* rightChannel, int numSamples)
{
    if (level_ < 0.001f)
        return;

    int delaySamples = getDelaySamples();

    for (int i = 0; i < numSamples; ++i)
    {
        int readPos = writePos_ - delaySamples;
        if (readPos < 0)
            readPos += bufferSize_;

        float delayedL = bufferL_[static_cast<size_t>(readPos)];
        float delayedR = bufferR_[static_cast<size_t>(readPos)];

        // Tape mode: lowpass the feedback
        if (delayType_ == DelayType::Tape)
        {
            float coeff = 0.3f;
            tapeLPState_ += coeff * (delayedL - tapeLPState_);
            delayedL = tapeLPState_;
            delayedR = tapeLPState_; // mono tape
        }

        float inputL = leftChannel[i];
        float inputR = (rightChannel != nullptr) ? rightChannel[i] : inputL;

        // Write to buffer with feedback
        if (delayType_ == DelayType::PingPong)
        {
            bufferL_[static_cast<size_t>(writePos_)] = inputL + delayedR * feedback_;
            bufferR_[static_cast<size_t>(writePos_)] = inputR + delayedL * feedback_;
        }
        else
        {
            bufferL_[static_cast<size_t>(writePos_)] = inputL + delayedL * feedback_;
            bufferR_[static_cast<size_t>(writePos_)] = inputR + delayedR * feedback_;
        }

        // Mix wet signal
        leftChannel[i] = inputL + delayedL * level_;
        if (rightChannel != nullptr)
            rightChannel[i] = inputR + delayedR * level_;

        writePos_ = (writePos_ + 1) % bufferSize_;
    }
}

void DelayEffect::reset()
{
    std::fill(bufferL_.begin(), bufferL_.end(), 0.0f);
    std::fill(bufferR_.begin(), bufferR_.end(), 0.0f);
    writePos_ = 0;
    tapeLPState_ = 0.0f;
}
