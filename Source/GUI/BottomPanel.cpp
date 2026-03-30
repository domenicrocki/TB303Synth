#include "BottomPanel.h"
#include "TB303LookAndFeel.h"

BottomPanel::BottomPanel(TB303AudioProcessor& processor)
    : processor_(processor)
{
    addAndMakeVisible(posLockButton_);

    runStopButton_.setToggleMode(true);
    runStopButton_.onClick = [this](bool on) {
        if (on) processor_.getSequencer().start();
        else    processor_.getSequencer().stop();
    };
    addAndMakeVisible(runStopButton_);

    addAndMakeVisible(keyboardToggle_);

    editButton_.setToggleMode(true);
    editButton_.onClick = [this](bool on) { editMode_ = on; editStep_ = 0; };
    addAndMakeVisible(editButton_);

    addAndMakeVisible(keyboard_);
    keyboard_.onNoteOn = [this](int midiNote) {
        if (editMode_) {
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            if (editStep_ < 16) {
                pattern.steps[editStep_].note = midiNote % 12 + 36;
                pattern.steps[editStep_].octave = (midiNote / 12) - 3;
                pattern.steps[editStep_].accent = accentButton_.isLEDOn();
                pattern.steps[editStep_].slide = slideButton_.isLEDOn();
                pattern.steps[editStep_].active = true;
                editStep_ = (editStep_ + 1) % 16;
                updateStepDisplay();
            }
        }
    };

    accentButton_.setToggleMode(true);
    addAndMakeVisible(accentButton_);
    slideButton_.setToggleMode(true);
    addAndMakeVisible(slideButton_);

    downButton_.setToggleMode(false);
    downButton_.onClick = [this](bool) {
        int oct = keyboard_.getBaseOctave();
        if (oct > 1) keyboard_.setBaseOctave(oct - 1);
    };
    addAndMakeVisible(downButton_);

    upButton_.setToggleMode(false);
    upButton_.onClick = [this](bool) {
        int oct = keyboard_.getBaseOctave();
        if (oct < 6) keyboard_.setBaseOctave(oct + 1);
    };
    addAndMakeVisible(upButton_);

    randomizeButton_.onClick = [this]() {
        undoPattern_ = processor_.getSequencer().getCurrentPatternRef();
        hasUndo_ = true;
        processor_.getSequencer().randomizeCurrentPattern();
        updateStepDisplay();
    };
    addAndMakeVisible(randomizeButton_);

    generateButton_.onClick = [this]() {
        if (hasUndo_) {
            processor_.getSequencer().getCurrentPatternRef() = undoPattern_;
            hasUndo_ = false; updateStepDisplay();
        }
    };
    addAndMakeVisible(generateButton_);

    for (int i = 0; i < 8; ++i) {
        auto* btn = new StepButton(i);
        btn->onToggle = [this](int step, bool active) {
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            pattern.steps[step].active = active;
        };
        stepButtons_.add(btn);
        addAndMakeVisible(btn);
    }

    modifyButton_.onClick = [this]() {
        if (hasUndo_) {
            processor_.getSequencer().getCurrentPatternRef() = undoPattern_;
            hasUndo_ = false; updateStepDisplay();
        }
    };
    addAndMakeVisible(modifyButton_);

    startTimerHz(30);
    updateStepDisplay();
}

BottomPanel::~BottomPanel() { stopTimer(); }

void BottomPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    TB303LookAndFeel::drawFuturisticPanel(g, bounds, TB303LookAndFeel::getNeonPink());

    // Note labels above keyboard
    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));

    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    int kbX = 185;
    int kbW = 480;
    float whiteKeyW = static_cast<float>(kbW) / 8.0f;
    int whiteIdx = 0;
    for (int i = 0; i < 13; ++i)
    {
        bool isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10);
        if (!isBlack)
        {
            float xPos = static_cast<float>(kbX) + static_cast<float>(whiteIdx) * whiteKeyW + whiteKeyW * 0.5f;
            g.drawText(noteNames[i], static_cast<int>(xPos - 10), 12, 20, 10, juce::Justification::centred);
            whiteIdx++;
        }
    }

    // Section labels
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(TB303LookAndFeel::getTextDim());
    g.drawText("KEYBOARD", 120, 12, 65, 12, juce::Justification::centred);

    // Notation symbols above accent/slide/down/up
    g.setFont(juce::Font(11.0f));
    g.setColour(TB303LookAndFeel::getTextDim());
    int symX = 690;
    int symGap = 60;
    g.drawText("3", symX + 14, 10, 20, 12, juce::Justification::centred);
    g.drawText("3", symX + symGap + 14, 10, 20, 12, juce::Justification::centred);

    // PATTERN / OCTAVE labels (neon pink)
    int bottomRowY = static_cast<int>(bounds.getHeight()) - 60;
    g.setColour(TB303LookAndFeel::getNeonPink());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("PATTERN", 270, bottomRowY + 42, 60, 12, juce::Justification::centred);
    g.drawText("OCTAVE", static_cast<int>(bounds.getWidth()) - 200, bottomRowY + 42, 60, 12, juce::Justification::centred);

    // Step numbers
    g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.6f));
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    int stepX = 340;
    int stepGap = 48;
    for (int i = 0; i < 8; ++i)
        g.drawText(juce::String(i + 1), stepX + i * stepGap, bottomRowY + 42, 40, 12, juce::Justification::centred);
}

void BottomPanel::resized()
{
    int w = getWidth();
    int h = getHeight();
    int topY = 18;
    int btnH = 50;
    int btnW = 55;

    posLockButton_.setBounds(12, topY, 85, 42);
    keyboardToggle_.setBounds(120, topY + 12, 55, 30);
    editButton_.setBounds(120, topY + 48, 55, 40);
    keyboard_.setBounds(185, topY + 6, 480, 85);

    int rbX = 690;
    int rbGap = 60;
    accentButton_.setBounds(rbX, topY, btnW, btnH);
    slideButton_.setBounds(rbX + rbGap, topY, btnW, btnH);
    downButton_.setBounds(rbX + rbGap * 2, topY, btnW, btnH);
    upButton_.setBounds(rbX + rbGap * 3, topY, btnW, btnH);

    randomizeButton_.setBounds(w - 110, topY, 95, 38);
    generateButton_.setBounds(w - 110, topY + 42, 95, 38);

    int bottomY = h - 60;
    runStopButton_.setBounds(12, topY + 50, 85, 45);

    int stepX = 340;
    int stepGap = 48;
    for (int i = 0; i < 8; ++i)
        stepButtons_[i]->setBounds(stepX + i * stepGap, bottomY, 40, 42);

    modifyButton_.setBounds(w - 110, bottomY, 95, 38);
}

void BottomPanel::timerCallback()
{
    if (processor_.getSequencer().isPlaying())
    {
        int cs = processor_.getSequencer().getCurrentStep();
        for (int i = 0; i < 8; ++i)
            stepButtons_[i]->setCurrent(cs == i || cs == i + 8);
    }
    else
    {
        for (int i = 0; i < 8; ++i)
            stepButtons_[i]->setCurrent(false);
    }
    runStopButton_.setLEDOn(processor_.getSequencer().isPlaying());
}

void BottomPanel::updateStepDisplay()
{
    auto& pattern = processor_.getSequencer().getCurrentPatternRef();
    for (int i = 0; i < 8; ++i)
        stepButtons_[i]->setActive(pattern.steps[i].active);
}
