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

    // White keys first
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

        // Classic white key look
        if (pressed)
        {
            g.setColour(juce::Colour(0xFFCCCCD0));
        }
        else
        {
            // White key with slight gradient for 3D effect
            juce::ColourGradient wGrad(juce::Colour(0xFFF8F8FA), keyBounds.getX(), keyBounds.getY(),
                                        juce::Colour(0xFFE8E8EC), keyBounds.getX(), keyBounds.getBottom(), false);
            g.setGradientFill(wGrad);
        }
        g.fillRect(keyBounds.reduced(0.5f));

        // White key border
        g.setColour(juce::Colour(0xFF606068));
        g.drawRect(keyBounds, 1.0f);
    }

    // Black keys on top
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

        // Classic black key with gradient
        {
            juce::ColourGradient bGrad(pressed ? juce::Colour(0xFF404048) : juce::Colour(0xFF303038),
                                        keyBounds.getX(), keyBounds.getY(),
                                        pressed ? juce::Colour(0xFF282830) : juce::Colour(0xFF181820),
                                        keyBounds.getX(), keyBounds.getBottom(), false);
            g.setGradientFill(bGrad);
        }
        g.fillRect(keyBounds);

        // Slight highlight at top of black key
        g.setColour(juce::Colour(0xFF484850));
        g.drawLine(keyBounds.getX() + 1, keyBounds.getY() + 1,
                   keyBounds.getRight() - 1, keyBounds.getY() + 1, 0.7f);

        // Black key border
        g.setColour(juce::Colour(0xFF101018));
        g.drawRect(keyBounds, 1.0f);
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
