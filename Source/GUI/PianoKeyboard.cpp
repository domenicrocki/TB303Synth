#include "PianoKeyboard.h"
#include "TB303LookAndFeel.h"

PianoKeyboard::PianoKeyboard()
{
    buildKeyLayout();
}

void PianoKeyboard::buildKeyLayout()
{
    keys_.clear();
    const int whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11, 12 };
    const int blackNotes[] = { 1, 3, 6, 8, 10 };
    const int blackPositions[] = { 0, 1, 3, 4, 5 };

    float totalWidth = 1.0f;
    float whiteKeyWidth = totalWidth / 8.0f;
    float blackKeyWidth = whiteKeyWidth * 0.6f;

    for (int i = 0; i < 8; ++i)
    {
        KeyInfo k;
        k.bounds = juce::Rectangle<float>(static_cast<float>(i) * whiteKeyWidth, 0.0f, whiteKeyWidth, 1.0f);
        k.isBlack = false;
        k.noteOffset = whiteNotes[i];
        keys_.push_back(k);
    }

    for (int i = 0; i < 5; ++i)
    {
        KeyInfo k;
        float x = (static_cast<float>(blackPositions[i]) + 1.0f) * whiteKeyWidth - blackKeyWidth * 0.5f;
        k.bounds = juce::Rectangle<float>(x, 0.0f, blackKeyWidth, 0.62f);
        k.isBlack = true;
        k.noteOffset = blackNotes[i];
        keys_.push_back(k);
    }
}

void PianoKeyboard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // White keys
    for (auto& key : keys_)
    {
        if (key.isBlack) continue;
        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight());

        int midiNote = baseOctave_ * 12 + key.noteOffset;
        bool pressed = (midiNote == pressedNote_);

        // Futuristic white keys - dark with cyan border when pressed
        g.setColour(pressed ? juce::Colour(0xFF2A2A3A) : juce::Colour(0xFFD8D8E0));
        g.fillRect(keyBounds.reduced(0.5f));
        g.setColour(pressed ? TB303LookAndFeel::getNeonCyan().withAlpha(0.5f) : juce::Colour(0xFF404050));
        g.drawRect(keyBounds, 1.0f);
    }

    // Black keys
    for (auto& key : keys_)
    {
        if (!key.isBlack) continue;
        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight());

        int midiNote = baseOctave_ * 12 + key.noteOffset;
        bool pressed = (midiNote == pressedNote_);

        g.setColour(pressed ? juce::Colour(0xFF1A1A30) : juce::Colour(0xFF1A1A22));
        g.fillRect(keyBounds);
        if (pressed)
        {
            g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.3f));
            g.drawRect(keyBounds, 1.0f);
        }
    }

    // Note labels
    g.setFont(juce::Font(8.0f));
    const char* noteNames[] = { "C", "D", "E", "F", "G", "A", "B", "C" };
    int idx = 0;
    for (auto& key : keys_)
    {
        if (key.isBlack) continue;
        auto keyBounds = juce::Rectangle<float>(
            bounds.getX() + key.bounds.getX() * bounds.getWidth(),
            bounds.getY() + key.bounds.getY() * bounds.getHeight(),
            key.bounds.getWidth() * bounds.getWidth(),
            key.bounds.getHeight() * bounds.getHeight());
        g.setColour(juce::Colour(0xFF606070));
        g.drawText(noteNames[idx], keyBounds.withTrimmedTop(keyBounds.getHeight() * 0.78f),
                   juce::Justification::centred);
        idx++;
    }
}

int PianoKeyboard::getNoteForPosition(juce::Point<float> pos) const
{
    auto bounds = getLocalBounds().toFloat();
    float normX = (pos.x - bounds.getX()) / bounds.getWidth();
    float normY = (pos.y - bounds.getY()) / bounds.getHeight();

    for (int i = static_cast<int>(keys_.size()) - 1; i >= 0; --i)
    {
        if (!keys_[static_cast<size_t>(i)].isBlack) continue;
        if (keys_[static_cast<size_t>(i)].bounds.contains(normX, normY))
            return baseOctave_ * 12 + keys_[static_cast<size_t>(i)].noteOffset;
    }

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
