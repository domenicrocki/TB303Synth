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
    TB303LookAndFeel::paintDarkBg(g, bounds);

    // --- Top row labels ---
    g.setColour(TB303LookAndFeel::textLt());
    g.setFont(juce::Font(8.0f, juce::Font::bold));

    // POSITION LOCK TO DAW frame
    auto posFrame = juce::Rectangle<float>(8.0f, 12.0f, 95.0f, 50.0f);
    g.setColour(juce::Colour(0xFF505058));
    g.drawRoundedRectangle(posFrame, 3.0f, 1.0f);
    g.setColour(TB303LookAndFeel::textLt());
    g.setFont(juce::Font(7.0f, juce::Font::bold));
    g.drawText("POSITION LOCK", 12, 14, 87, 10, juce::Justification::centred);
    g.drawText("TO DAW", 12, 23, 87, 10, juce::Justification::centred);

    // KEYBOARD label
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(TB303LookAndFeel::textLt());
    g.drawText("KEYBOARD", 112, 12, 65, 12, juce::Justification::centred);

    // EDIT label
    g.drawText("EDIT", 112, 62, 65, 12, juce::Justification::centred);

    // Note name labels above keyboard
    g.setColour(TB303LookAndFeel::textLt());
    g.setFont(juce::Font(8.0f, juce::Font::bold));

    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    int kbX = 195;
    int kbW = 500;
    float whiteKeyW = static_cast<float>(kbW) / 8.0f;

    // Map all 13 note names to their x positions
    // White keys: C(0), D(1), E(2), F(3), G(4), A(5), B(6), C(7) -> white key indices
    // Black keys positioned between whites
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
        else
        {
            // Black key note names - position between white keys
            // Find the corresponding position
            float prevWhite = 0.0f;
            if (i == 1) prevWhite = 0.0f; // between C and D
            else if (i == 3) prevWhite = 1.0f; // between D and E
            else if (i == 6) prevWhite = 3.0f; // between F and G
            else if (i == 8) prevWhite = 4.0f; // between G and A
            else if (i == 10) prevWhite = 5.0f; // between A and B
            float xPos = static_cast<float>(kbX) + (prevWhite + 1.0f) * whiteKeyW;
            g.drawText(noteNames[i], static_cast<int>(xPos - 10), 12, 20, 10, juce::Justification::centred);
        }
    }

    // Triplet marks above ACCENT and SLIDE
    g.setFont(juce::Font(11.0f));
    g.setColour(TB303LookAndFeel::textLt());
    int symX = 720;
    int symGap = 62;
    g.drawText("3", symX + 14, 10, 20, 12, juce::Justification::centred);
    g.drawText("3", symX + symGap + 14, 10, 20, 12, juce::Justification::centred);

    // Note symbols above DOWN and UP
    g.setFont(juce::Font(10.0f));
    // Down arrow / note symbol
    {
        juce::Path arrow;
        float ax = static_cast<float>(symX + symGap * 2 + 22);
        arrow.addTriangle(ax, 20.0f, ax + 5.0f, 12.0f, ax - 5.0f, 12.0f);
        g.fillPath(arrow);
    }
    // Up arrow / note symbol
    {
        juce::Path arrow;
        float ax = static_cast<float>(symX + symGap * 3 + 22);
        arrow.addTriangle(ax, 10.0f, ax + 5.0f, 18.0f, ax - 5.0f, 18.0f);
        g.fillPath(arrow);
    }

    // RANDOMIZE frame
    float rightFrameX = bounds.getWidth() - 120.0f;
    auto randFrame = juce::Rectangle<float>(rightFrameX, 12.0f, 108.0f, 44.0f);
    g.setColour(juce::Colour(0xFF505058));
    g.drawRoundedRectangle(randFrame, 3.0f, 1.0f);

    // GENERATE/UNDO frame
    auto genFrame = juce::Rectangle<float>(rightFrameX, 60.0f, 108.0f, 44.0f);
    g.drawRoundedRectangle(genFrame, 3.0f, 1.0f);

    // --- Bottom row ---
    int bottomRowY = static_cast<int>(bounds.getHeight()) - 100;

    // RUN/STOP frame area (already has LED button)

    // PATTERN label (orange/red)
    g.setColour(TB303LookAndFeel::accentOr());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("PATTERN", 200, bottomRowY + 52, 60, 12, juce::Justification::centred);

    // Step numbers in orange below step buttons
    g.setColour(TB303LookAndFeel::accentOr());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    int stepX = 280;
    int stepGap = 52;
    for (int i = 0; i < 8; ++i)
        g.drawText(juce::String(i + 1), stepX + i * stepGap, bottomRowY + 52, 44, 12, juce::Justification::centred);

    // OCTAVE label (orange/red)
    g.setColour(TB303LookAndFeel::accentOr());
    g.drawText("OCTAVE", static_cast<int>(bounds.getWidth()) - 220, bottomRowY + 52, 60, 12, juce::Justification::centred);

    // MODIFY/UNDO frame
    auto modFrame = juce::Rectangle<float>(rightFrameX, static_cast<float>(bottomRowY) + 8.0f, 108.0f, 44.0f);
    g.setColour(juce::Colour(0xFF505058));
    g.drawRoundedRectangle(modFrame, 3.0f, 1.0f);
}

void BottomPanel::resized()
{
    int w = getWidth();
    int h = getHeight();
    int topY = 18;
    int btnH = 50;
    int btnW = 55;

    // Top row: Position lock
    posLockButton_.setBounds(14, topY + 18, 82, 30);

    // Keyboard toggle and edit
    keyboardToggle_.setBounds(118, topY + 6, 60, 30);
    editButton_.setBounds(118, topY + 50, 60, 40);

    // Piano keyboard
    keyboard_.setBounds(195, topY + 6, 500, 90);

    // Accent, Slide, Down, Up buttons
    int rbX = 720;
    int rbGap = 62;
    accentButton_.setBounds(rbX, topY, btnW, btnH);
    slideButton_.setBounds(rbX + rbGap, topY, btnW, btnH);
    downButton_.setBounds(rbX + rbGap * 2, topY, btnW, btnH);
    upButton_.setBounds(rbX + rbGap * 3, topY, btnW, btnH);

    // Far right: Randomize and Generate/Undo
    float rightX = static_cast<float>(w) - 116.0f;
    randomizeButton_.setBounds(static_cast<int>(rightX) + 4, topY, 100, 38);
    generateButton_.setBounds(static_cast<int>(rightX) + 4, topY + 46, 100, 38);

    // Bottom row
    int bottomY = h - 96;

    // RUN/STOP
    runStopButton_.setBounds(14, bottomY, 85, 50);

    // Step buttons
    int stepX = 280;
    int stepGap = 52;
    for (int i = 0; i < 8; ++i)
        stepButtons_[i]->setBounds(stepX + i * stepGap, bottomY, 44, 46);

    // MODIFY/UNDO
    modifyButton_.setBounds(static_cast<int>(rightX) + 4, bottomY + 4, 100, 38);
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
