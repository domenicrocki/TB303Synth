#include "StepSequencer.h"

StepSequencer::StepSequencer() {}

void StepSequencer::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    updateTiming();
}

void StepSequencer::updateTiming()
{
    double beatsPerSecond = bpm_ / 60.0;
    double stepsPerBeat = 4.0; // 1/16 notes

    switch (scale_)
    {
        case Scale::SixteenthTriplet:
            stepsPerBeat = 6.0; // 1/16T
            break;
        case Scale::Sixteenth:
            stepsPerBeat = 4.0; // 1/16
            break;
        case Scale::ThirtySecond:
            stepsPerBeat = 8.0; // 1/32
            break;
    }

    double stepsPerSecond = beatsPerSecond * stepsPerBeat;
    samplesPerStep_ = sampleRate_ / stepsPerSecond;
}

void StepSequencer::processBlock(int numSamples, TB303Voice& voice)
{
    if (!playing_)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        sampleCounter_ += 1.0;

        // Apply shuffle: delay even steps
        double currentStepLength = samplesPerStep_;
        if (currentStep_ % 2 == 1 && shuffle_ > 0.0f)
        {
            currentStepLength = samplesPerStep_ * (1.0 + static_cast<double>(shuffle_) * 0.5);
        }
        else if (currentStep_ % 2 == 0 && shuffle_ > 0.0f)
        {
            currentStepLength = samplesPerStep_ * (1.0 - static_cast<double>(shuffle_) * 0.5);
        }

        if (sampleCounter_ >= currentStepLength)
        {
            sampleCounter_ -= currentStepLength;

            // Trigger current step
            auto& pattern = getCurrentPatternRef();
            auto& step = pattern.steps[currentStep_];

            if (step.active)
            {
                int midiNote = step.note + (step.octave * 12);
                voice.noteOn(midiNote, step.accent, step.slide);
            }
            else
            {
                voice.noteOff();
            }

            // Advance to next step
            currentStep_ = getNextStep();
        }
    }
}

int StepSequencer::getNextStep()
{
    auto& pattern = getCurrentPatternRef();
    int numSteps = pattern.numSteps;

    switch (playMode_)
    {
        case PlayMode::Forward:
            return (currentStep_ + 1) % numSteps;

        case PlayMode::Reverse:
            return (currentStep_ - 1 + numSteps) % numSteps;

        case PlayMode::FwdAndRev:
        {
            int next;
            if (forward_)
            {
                next = currentStep_ + 1;
                if (next >= numSteps)
                {
                    forward_ = false;
                    next = numSteps - 2;
                    if (next < 0) next = 0;
                }
            }
            else
            {
                next = currentStep_ - 1;
                if (next < 0)
                {
                    forward_ = true;
                    next = 1;
                    if (next >= numSteps) next = 0;
                }
            }
            return next;
        }

        case PlayMode::Invert:
        {
            // Play pattern with inverted note order
            return (currentStep_ + 1) % numSteps;
        }

        case PlayMode::Random:
            return random_.nextInt(numSteps);
    }

    return (currentStep_ + 1) % numSteps;
}

void StepSequencer::start()
{
    playing_ = true;
    currentStep_ = 0;
    sampleCounter_ = samplesPerStep_; // Trigger first note immediately
    forward_ = true;
}

void StepSequencer::stop()
{
    playing_ = false;
    currentStep_ = 0;
}

void StepSequencer::setTempo(double bpm)
{
    bpm_ = juce::jlimit(20.0, 300.0, bpm);
    updateTiming();
}

void StepSequencer::setPlayMode(PlayMode mode) { playMode_ = mode; forward_ = true; }
void StepSequencer::setScale(Scale scale) { scale_ = scale; updateTiming(); }
void StepSequencer::setShuffle(float amount) { shuffle_ = juce::jlimit(0.0f, 1.0f, amount); }
void StepSequencer::setCurrentBank(int bank) { currentBank_ = juce::jlimit(0, PatternData::NUM_BANKS - 1, bank); }
void StepSequencer::setCurrentPattern(int pattern) { currentPattern_ = juce::jlimit(0, PatternData::PATTERNS_PER_BANK - 1, pattern); }

void StepSequencer::randomizeCurrentPattern()
{
    getCurrentPatternRef().randomize(random_);
}

void StepSequencer::setHostSync(bool enabled) { hostSync_ = enabled; }

void StepSequencer::setHostPosition(double ppqPosition, double bpm, bool isPlaying)
{
    hostPPQ_ = ppqPosition;
    if (hostSync_)
    {
        bpm_ = bpm;
        updateTiming();
        if (isPlaying != hostPlaying_)
        {
            hostPlaying_ = isPlaying;
            if (isPlaying)
                start();
            else
                stop();
        }
    }
}

Pattern& StepSequencer::getCurrentPatternRef()
{
    return patternData_.getPattern(currentBank_, currentPattern_);
}
