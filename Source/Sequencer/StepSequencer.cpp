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
            stepsPerBeat = 6.0;
            break;
        case Scale::Sixteenth:
            stepsPerBeat = 4.0;
            break;
        case Scale::ThirtySecond:
            stepsPerBeat = 8.0;
            break;
    }

    double stepsPerSecond = beatsPerSecond * stepsPerBeat;
    samplesPerStep_ = sampleRate_ / stepsPerSecond;
}

void StepSequencer::processBlock(int numSamples, TB303Voice& voice)
{
    if (!playing_)
        return;

    // Calculate shuffle-adjusted step length once (not per-sample)
    double stepLength = samplesPerStep_;
    if (shuffle_ > 0.0f)
    {
        if (currentStep_ % 2 == 1)
            stepLength = samplesPerStep_ * (1.0 + static_cast<double>(shuffle_) * 0.7);
        else
            stepLength = samplesPerStep_ * (1.0 - static_cast<double>(shuffle_) * 0.7);

        stepLength = juce::jmax(stepLength, samplesPerStep_ * 0.2);
    }

    // Gate length: 75% of step for normal notes, 100% for slide notes
    double gateLength = stepLength * 0.75;
    auto& pattern = getCurrentPatternRef();
    if (currentStep_ < pattern.numSteps)
    {
        int nextStep = peekNextStep();
        if (nextStep < pattern.numSteps && pattern.steps[nextStep].slide)
            gateLength = stepLength; // hold through to next note for slide
    }

    for (int i = 0; i < numSamples; ++i)
    {
        sampleCounter_ += 1.0;

        // Note-off at gate end (only if not sliding into next note)
        if (noteIsOn_ && sampleCounter_ >= gateLength && !slidingToNext_)
        {
            voice.noteOff();
            noteIsOn_ = false;
        }

        // Step advance
        if (sampleCounter_ >= stepLength)
        {
            sampleCounter_ -= stepLength;

            // Advance step
            currentStep_ = getNextStep();

            // Recalculate shuffle for new step
            if (shuffle_ > 0.0f)
            {
                if (currentStep_ % 2 == 1)
                    stepLength = samplesPerStep_ * (1.0 + static_cast<double>(shuffle_) * 0.7);
                else
                    stepLength = samplesPerStep_ * (1.0 - static_cast<double>(shuffle_) * 0.7);
                stepLength = juce::jmax(stepLength, samplesPerStep_ * 0.2);
            }
            else
            {
                stepLength = samplesPerStep_;
            }

            // Check if next step has slide (for gate length)
            int upcoming = peekNextStep();
            slidingToNext_ = (upcoming < pattern.numSteps && pattern.steps[upcoming].slide
                              && pattern.steps[upcoming].active);
            gateLength = slidingToNext_ ? stepLength : stepLength * 0.75;

            // Trigger current step
            auto& step = pattern.steps[currentStep_];

            if (step.active)
            {
                int midiNote = step.note + (step.octave * 12);
                midiNote = juce::jlimit(12, 120, midiNote);
                voice.noteOn(midiNote, step.accent, step.slide);
                noteIsOn_ = true;
            }
            else
            {
                // Rest: release previous note
                if (noteIsOn_)
                {
                    voice.noteOff();
                    noteIsOn_ = false;
                }
            }
        }
    }
}

int StepSequencer::peekNextStep() const
{
    auto& pattern = patternData_.getPattern(currentBank_, currentPattern_);
    int numSteps = pattern.numSteps;

    switch (playMode_)
    {
        case PlayMode::Forward:
            return (currentStep_ + 1) % numSteps;

        case PlayMode::Reverse:
            return (currentStep_ - 1 + numSteps) % numSteps;

        case PlayMode::FwdAndRev:
        {
            if (forward_)
            {
                int next = currentStep_ + 1;
                return (next >= numSteps) ? numSteps - 2 : next;
            }
            else
            {
                int next = currentStep_ - 1;
                return (next < 0) ? 1 : next;
            }
        }

        case PlayMode::Invert:
            return (numSteps - 1 - ((currentStep_ + 1) % numSteps));

        case PlayMode::Random:
            return -1; // can't predict

        default:
            return (currentStep_ + 1) % numSteps;
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
                    next = juce::jmax(0, numSteps - 2);
                }
            }
            else
            {
                next = currentStep_ - 1;
                if (next < 0)
                {
                    forward_ = true;
                    next = juce::jmin(1, numSteps - 1);
                }
            }
            return next;
        }

        case PlayMode::Invert:
        {
            // Play the pattern in reverse note order while stepping forward
            int forwardStep = (currentStep_ + 1) % numSteps;
            return (numSteps - 1 - forwardStep);
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
    noteIsOn_ = false;
    slidingToNext_ = false;
}

void StepSequencer::stop()
{
    playing_ = false;
    currentStep_ = 0;
    noteIsOn_ = false;
    slidingToNext_ = false;
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
