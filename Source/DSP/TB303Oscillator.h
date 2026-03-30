#pragma once
#include <JuceHeader.h>

class TB303Oscillator
{
public:
    enum class Waveform { Sawtooth, Square };

    TB303Oscillator();

    void prepare(double sampleRate);
    void setFrequency(float freq, bool glide = false);
    void setWaveform(Waveform wf);
    void setTuning(float semitones);
    float process();
    void reset();

private:
    float polyBlepSaw(float phase);
    float polyBlepSquare(float phase);
    float polyBlep(float t);

    double sampleRate_ = 44100.0;
    float phase_ = 0.0f;
    float phaseIncrement_ = 0.0f;
    float targetFrequency_ = 440.0f;
    float currentFrequency_ = 440.0f;
    float tuningOffset_ = 0.0f;
    Waveform waveform_ = Waveform::Sawtooth;
    bool gliding_ = false;
    float glideRate_ = 0.0f;

    static constexpr float GLIDE_TIME_MS = 60.0f;
};
