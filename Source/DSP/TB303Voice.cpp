#include "TB303Voice.h"

TB303Voice::TB303Voice() {}

void TB303Voice::prepare(double sampleRate)
{
    oscillator_.prepare(sampleRate);
    filter_.prepare(sampleRate);
    filterEnvelope_.prepare(sampleRate);
    ampEnvelope_.prepare(sampleRate);
}

void TB303Voice::noteOn(int midiNote, bool accent, bool slide)
{
    float freq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    oscillator_.setFrequency(freq, slide);
    currentAccentLevel_ = accent ? 1.0f : 0.0f;
    filterEnvelope_.trigger();
    ampEnvelope_.trigger();
    noteActive_ = true;
}

void TB303Voice::noteOff()
{
    filterEnvelope_.release();
    ampEnvelope_.release();
    noteActive_ = false;
}

float TB303Voice::process()
{
    if (!filterEnvelope_.isActive() && !ampEnvelope_.isActive())
        return 0.0f;

    float oscOut = oscillator_.process();

    // Filter envelope modulation
    float envValue = filterEnvelope_.process();
    float envDepth = envModAmount_ * ENV_MOD_RANGE;
    float accentBoost = currentAccentLevel_ * accentAmount_ * ACCENT_CUTOFF_BOOST;
    float modulatedCutoff = baseCutoff_ + envValue * (envDepth + accentBoost);
    modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
    filter_.setCutoff(modulatedCutoff);

    float filtered = filter_.process(oscOut);

    // Amp envelope with accent volume boost
    float ampEnv = ampEnvelope_.process();
    float accentVolumeBoost = 1.0f + currentAccentLevel_ * accentAmount_ * ACCENT_VOLUME_BOOST;
    float output = filtered * ampEnv * accentVolumeBoost;

    return output;
}

void TB303Voice::reset()
{
    oscillator_.reset();
    filter_.reset();
    filterEnvelope_.reset();
    ampEnvelope_.reset();
    noteActive_ = false;
}

bool TB303Voice::isActive() const
{
    return filterEnvelope_.isActive() || ampEnvelope_.isActive();
}

void TB303Voice::setWaveform(TB303Oscillator::Waveform wf) { oscillator_.setWaveform(wf); }
void TB303Voice::setTuning(float semitones) { oscillator_.setTuning(semitones); }
void TB303Voice::setCutoff(float cutoffHz) { baseCutoff_ = cutoffHz; }
void TB303Voice::setResonance(float resonance) { filter_.setResonance(resonance); }
void TB303Voice::setEnvMod(float amount) { envModAmount_ = amount; }
void TB303Voice::setDecay(float decayMs) { filterEnvelope_.setDecay(decayMs); ampEnvelope_.setDecay(decayMs * 1.5f); }
void TB303Voice::setAccentAmount(float amount) { accentAmount_ = amount; }
