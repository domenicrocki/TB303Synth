#include "PianoKeyboard.h"
#include "TB303LookAndFeel.h"

PianoKeyboard::PianoKeyboard()
{
    buildKeyLayout();
}

void PianoKeyboard::buildKeyLayout()
{
    keys_.clear();
    // 13 keys: C C# D D# E F F# G G# A A# B C
    // White keys: C D E F G A B C = 8 white keys
    // Black keys: C# D# F# G# A# = 5 black keys

    // Note offsets from base C
    const int whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11, 12 };
    const int blackNotes[] = { 1, 3, 6, 8, 10 };
    const int blackPositions[] = { 0, 1, 3, 4, 5 }; // which white key gap

    float totalWidth = 1.0f;
    float whiteKeyWidth = totalWidth / 8.0f;
    float blackKeyWidth = whiteKeyWidth * 0.6f;
    float whiteKeyHeight = 1.0f;
    float blackKeyHeight = 0.6f;

    // White keys
    for (int i = 0; i < 8; ++i)
    {
        KeyInfo k;
        k.bounds = juce::Rectangle<float>(static_cast<float>(i) * whiteKeyWidth, 0.0f, whiteKeyWidth, whiteKeyHeight);
        k.isBlack = false;
        k.noteOffset = whiteNotes[i];
        keys_.push_back(k);
    }

    // Black keys (added after so they draw on top)
    for (int i = 0; i < 5; ++i)
    {
        KeyInfo k;
        float x = (static_cast<float>(blackPositions[i]) + 1.0f) * whiteKeyWidth - blackKeyWidth * 0.5f;
        k.bounds = juce::Rectangle<float>(x, 0.0f, blackKeyWidth, blackKeyHeight);
        k.isBlack = true;
        k.noteOffset = blackNotes[i];
        keys_.push_back(k);
    }
}

void PianoKeyboard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Draw white keys first
    for (auto& key : keys_)
    {
        if (key.isBlack) continue;

        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight()
        );

        int midiNote = baseOctave_ * 12 + key.noteOffset;
        bool pressed = (midiNote == pressedNote_);

        g.setColour(pressed ? juce::Colour(0xFFD0D0E0) : juce::Colours::white);
        g.fillRect(keyBounds.reduced(0.5f));
        g.setColour(juce::Colour(0xFF404048));
        g.drawRect(keyBounds, 1.0f);
    }

    // Draw black keys on top
    for (auto& key : keys_)
    {
        if (!key.isBlack) continue;

        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight()
        );

        int midiNote = baseOctave_ * 12 + key.noteOffset;
        bool pressed = (midiNote == pressedNote_);

        g.setColour(pressed ? juce::Colour(0xFF505058) : juce::Colour(0xFF1A1A20));
        g.fillRect(keyBounds);
        g.setColour(juce::Colour(0xFF000000));
        g.drawRect(keyBounds, 1.0f);
    }

    // Key labels on white keys
    g.setFont(juce::Font(9.0f));
    const char* noteNames[] = { "C", "D", "E", "F", "G", "A", "B", "C" };
    int idx = 0;
    for (auto& key : keys_)
    {
        if (key.isBlack) continue;
        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight()
        );
        g.setColour(juce::Colour(0xFF808080));
        g.drawText(noteNames[idx], keyBounds.withTrimmedTop(keyBounds.getHeight() * 0.75f),
                   juce::Justification::centred);
        idx++;
    }
}

int PianoKeyboard::getNoteForPosition(juce::Point<float> pos) const
{
    auto bounds = getLocalBounds().toFloat();
    float normX = (pos.x - bounds.getX()) / bounds.getWidth();
    float normY = (pos.y - bounds.getY()) / bounds.getHeight();

    // Check black keys first (they're on top)
    for (int i = static_cast<int>(keys_.size()) - 1; i >= 0; --i)
    {
        if (!keys_[static_cast<size_t>(i)].isBlack) continue;
        if (keys_[static_cast<size_t>(i)].bounds.contains(normX, normY))
            return baseOctave_ * 12 + keys_[static_cast<size_t>(i)].noteOffset;
    }

    // Then white keys
    for (auto& key : keys_)
    {
        if (key.isBlack) continue;
        if (key.bounds.contains(normX, normY))
            return baseOctave_ * 12 + key.noteOffset;
    }

    return -1;
}

void PianoKeyboard::mouseDown(const juce::MouseEvent& e)
{
    int note = getNoteForPosition(e.position);
    if (note >= 0)
    {
        pressedNote_ = note;
        repaint();
        if (onNoteOn) onNoteOn(note);
    }
}

void PianoKeyboard::mouseUp(const juce::MouseEvent& /*e*/)
{
    if (pressedNote_ >= 0)
    {
        int note = pressedNote_;
        pressedNote_ = -1;
        repaint();
        if (onNoteOff) onNoteOff(note);
    }
}
