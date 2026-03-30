#pragma once
#include <JuceHeader.h>
#include "TB303Oscillator.h"
#include "TB303Filter.h"
#include "TB303Envelope.h"

class TB303Voice
{
public:
    TB303Voice();

    void prepare(double sampleRate);
    void noteOn(int midiNote, bool accent, bool slide);
    void noteOff();
    float process();
    void reset();
    bool isActive() const;

    // Parameters
    void setWaveform(TB303Oscillator::Waveform wf);
    void setTuning(float semitones);
    void setCutoff(float cutoffHz);
    void setResonance(float resonance);
    void setEnvMod(float amount);
    void setDecay(float decayMs);
    void setAccentAmount(float amount);

private:
    TB303Oscillator oscillator_;
    TB303Filter filter_;
    TB303Envelope filterEnvelope_;
    TB303Envelope ampEnvelope_;

    float baseCutoff_ = 1000.0f;
    float envModAmount_ = 0.5f;
    float accentAmount_ = 0.5f;
    float currentAccentLevel_ = 0.0f;
    bool noteActive_ = false;

    static constexpr float ENV_MOD_RANGE = 8000.0f;
    static constexpr float ACCENT_CUTOFF_BOOST = 4000.0f;
    static constexpr float ACCENT_VOLUME_BOOST = 0.5f;
};
