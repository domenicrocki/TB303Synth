#pragma once
#include <JuceHeader.h>

class PianoKeyboard : public juce::Component
{
public:
    PianoKeyboard();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    void setBaseOctave(int octave) { baseOctave_ = octave; repaint(); }
    int getBaseOctave() const { return baseOctave_; }

    std::function<void(int)> onNoteOn;
    std::function<void(int)> onNoteOff;

private:
    int getNoteForPosition(juce::Point<float> pos) const;

    int baseOctave_ = 3;
    int pressedNote_ = -1;

    struct KeyInfo {
        juce::Rectangle<float> bounds;
        bool isBlack;
        int noteOffset;
    };
    std::vector<KeyInfo> keys_;
    void buildKeyLayout();
};
