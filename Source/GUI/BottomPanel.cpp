#include "BottomPanel.h"
#include "TB303LookAndFeel.h"

BottomPanel::BottomPanel(TB303AudioProcessor& processor)
    : processor_(processor)
{
    addAndMakeVisible(posLockButton_);
    addAndMakeVisible(keyboardToggle_);
    addAndMakeVisible(keyboard_);

    // Run/Stop
    runStopButton_.setToggleMode(true);
    runStopButton_.onClick = [this](bool on)
    {
        if (on)
            processor_.getSequencer().start();
        else
            processor_.getSequencer().stop();
    };
    addAndMakeVisible(runStopButton_);

    // Edit mode
    editButton_.setToggleMode(true);
    editButton_.onClick = [this](bool on)
    {
        editMode_ = on;
        editStep_ = 0;
    };
    addAndMakeVisible(editButton_);

    // Accent toggle for step editing
    accentButton_.setToggleMode(true);
    addAndMakeVisible(accentButton_);

    // Slide toggle for step editing
    slideButton_.setToggleMode(true);
    addAndMakeVisible(slideButton_);

    // Octave down
    downButton_.setToggleMode(false);
    downButton_.onClick = [this](bool)
    {
        int oct = keyboard_.getBaseOctave();
        if (oct > 1) keyboard_.setBaseOctave(oct - 1);
    };
    addAndMakeVisible(downButton_);

    // Octave up
    upButton_.setToggleMode(false);
    upButton_.onClick = [this](bool)
    {
        int oct = keyboard_.getBaseOctave();
        if (oct < 6) keyboard_.setBaseOctave(oct + 1);
    };
    addAndMakeVisible(upButton_);

    // Step buttons (1-8 shown, representing pattern steps)
    for (int i = 0; i < 8; ++i)
    {
        auto* btn = new StepButton(i);
        btn->onToggle = [this](int step, bool active)
        {
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            pattern.steps[step].active = active;
        };
        stepButtons_.add(btn);
        addAndMakeVisible(btn);
    }

    // Keyboard callbacks
    keyboard_.onNoteOn = [this](int midiNote)
    {
        if (editMode_)
        {
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            if (editStep_ < 16)
            {
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

    keyboard_.onNoteOff = [this](int /*midiNote*/)
    {
        // For live play mode - handled by processor MIDI
    };

    // Randomize
    randomizeButton_.onClick = [this]()
    {
        // Save undo state
        undoPattern_ = processor_.getSequencer().getCurrentPatternRef();
        hasUndo_ = true;

        processor_.getSequencer().randomizeCurrentPattern();
        updateStepDisplay();
    };
    addAndMakeVisible(randomizeButton_);

    // Modify/Undo
    modifyButton_.onClick = [this]()
    {
        if (hasUndo_)
        {
            processor_.getSequencer().getCurrentPatternRef() = undoPattern_;
            hasUndo_ = false;
            updateStepDisplay();
        }
    };
    addAndMakeVisible(modifyButton_);

    // Start timer for step LED updates
    startTimerHz(30);

    // Initial step display
    updateStepDisplay();
}

BottomPanel::~BottomPanel()
{
    stopTimer();
}

void BottomPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark panel background
    g.setColour(TB303LookAndFeel::getDarkPanel());
    g.fillRect(bounds);

    // Lighter inset for keyboard area
    g.setColour(juce::Colour(0xFF3A3A40));
    g.fillRoundedRectangle(bounds.reduced(5.0f), 4.0f);

    // Section labels
    g.setColour(juce::Colour(0xFFB0B0B8));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("PATTERN", 370, static_cast<int>(bounds.getHeight()) - 15, 200, 12, juce::Justification::centredLeft);
    g.drawText("OCTAVE", static_cast<int>(bounds.getWidth()) - 190, static_cast<int>(bounds.getHeight()) - 15, 60, 12, juce::Justification::centred);

    // Note labels for keyboard section
    g.setFont(juce::Font(8.0f));
    const char* sharpNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    int kx = 175;
    int kSpacing = 32;
    for (int i = 0; i < 13; ++i)
    {
        g.drawText(sharpNames[i], kx + i * kSpacing / 2, 3, 30, 10, juce::Justification::centred);
    }
}

void BottomPanel::resized()
{
    auto bounds = getLocalBounds();
    int bh = bounds.getHeight();

    // Left section
    posLockButton_.setBounds(10, 10, 80, 40);
    keyboardToggle_.setBounds(10, 55, 80, 30);

    // Edit and Run/Stop
    runStopButton_.setBounds(95, 55, 55, 40);
    editButton_.setBounds(95, 10, 55, 40);

    // Piano keyboard
    keyboard_.setBounds(165, 15, 360, 75);

    // Accent, Slide, Down, Up
    int btnX = 540;
    int btnW = 55;
    accentButton_.setBounds(btnX, 15, btnW, 40);
    slideButton_.setBounds(btnX + btnW + 5, 15, btnW, 40);
    downButton_.setBounds(btnX + (btnW + 5) * 2, 15, btnW, 40);
    upButton_.setBounds(btnX + (btnW + 5) * 3, 15, btnW, 40);

    // Step buttons (pattern 1-8)
    int stepX = 370;
    int stepW = 35;
    for (int i = 0; i < 8; ++i)
    {
        stepButtons_[i]->setBounds(stepX + i * (stepW + 3), bh - 55, stepW, 40);
    }

    // Randomize / Modify buttons
    randomizeButton_.setBounds(bounds.getWidth() - 110, 10, 95, 35);
    modifyButton_.setBounds(bounds.getWidth() - 110, 50, 95, 35);
}

void BottomPanel::timerCallback()
{
    // Update step LEDs to show current position
    if (processor_.getSequencer().isPlaying())
    {
        int currentStep = processor_.getSequencer().getCurrentStep();
        for (int i = 0; i < 8; ++i)
        {
            stepButtons_[i]->setCurrent(currentStep == i || currentStep == i + 8);
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
            stepButtons_[i]->setCurrent(false);
    }

    // Update run/stop LED
    runStopButton_.setLEDOn(processor_.getSequencer().isPlaying());
}

void BottomPanel::updateStepDisplay()
{
    auto& pattern = processor_.getSequencer().getCurrentPatternRef();
    for (int i = 0; i < 8; ++i)
    {
        stepButtons_[i]->setActive(pattern.steps[i].active);
    }
}
