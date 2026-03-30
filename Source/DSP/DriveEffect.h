#pragma once
#include <JuceHeader.h>

class DriveEffect
{
public:
    enum class DriveType { Soft, Hard, Tube };

    DriveEffect();

    void prepare(double sampleRate);
    void setDriveType(DriveType type);
    void setDepth(float depth);
    void setTone(float tone);
    float process(float input);
    void reset();

private:
    double sampleRate_ = 44100.0;
    DriveType driveType_ = DriveType::Soft;
    float depth_ = 0.0f;
    float tone_ = 0.5f;

    // Tone filter (simple one-pole lowpass)
    float toneFilterState_ = 0.0f;
    float toneCoeff_ = 0.5f;

    void updateToneFilter();
};
