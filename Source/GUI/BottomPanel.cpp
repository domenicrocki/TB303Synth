#include "BottomPanel.h"
#include "TB303LookAndFeel.h"

BottomPanel::BottomPanel(TB303AudioProcessor& processor)
    : processor_(processor)
{
    // Transport
    runStopButton_.setToggleMode(true);
    runStopButton_.setAccentColor(TB303Colors::pink());
    runStopButton_.onClick = [this](bool on) {
        if (on) processor_.getSequencer().start();
        else    processor_.getSequencer().stop();
    };
    addAndMakeVisible(runStopButton_);

    recordButton_.setToggleMode(true);
    recordButton_.setAccentColor(TB303Colors::pink());
    recordButton_.onClick = [this](bool on) {
        editMode_ = on;
        editStep_ = 0;
    };
    addAndMakeVisible(recordButton_);

    // Keyboard
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

    // Octave
    downButton_.setToggleMode(false);
    downButton_.setAccentColor(TB303Colors::pink());
    downButton_.onClick = [this](bool) {
        int oct = keyboard_.getBaseOctave();
        if (oct > 1) keyboard_.setBaseOctave(oct - 1);
    };
    addAndMakeVisible(downButton_);

    upButton_.setToggleMode(false);
    upButton_.setAccentColor(TB303Colors::pink());
    upButton_.onClick = [this](bool) {
        int oct = keyboard_.getBaseOctave();
        if (oct < 6) keyboard_.setBaseOctave(oct + 1);
    };
    addAndMakeVisible(upButton_);

    // Articulation
    accentButton_.setToggleMode(true);
    accentButton_.setAccentColor(TB303Colors::pink());
    addAndMakeVisible(accentButton_);
    slideButton_.setToggleMode(true);
    slideButton_.setAccentColor(TB303Colors::pink());
    addAndMakeVisible(slideButton_);

    // Pattern actions (under painted "RANDOMIZE" label)

    // Generate/Undo: creates a completely new random pattern, Undo reverts
    generateUndoButton_.onClick = [this]() {
        if (hasGenerateUndo_) {
            processor_.getSequencer().getCurrentPatternRef() = generateUndoPattern_;
            hasGenerateUndo_ = false;
        } else {
            generateUndoPattern_ = processor_.getSequencer().getCurrentPatternRef();
            hasGenerateUndo_ = true;
            processor_.getSequencer().randomizeCurrentPattern();
        }
        updateStepDisplay();
    };
    addAndMakeVisible(generateUndoButton_);

    // Modify/Undo: randomly changes only some steps of the current pattern, Undo reverts
    modifyUndoButton_.onClick = [this]() {
        if (hasModifyUndo_) {
            processor_.getSequencer().getCurrentPatternRef() = modifyUndoPattern_;
            hasModifyUndo_ = false;
        } else {
            modifyUndoPattern_ = processor_.getSequencer().getCurrentPatternRef();
            hasModifyUndo_ = true;
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            juce::Random rng;
            for (int i = 0; i < 16; ++i) {
                if (rng.nextFloat() < 0.35f) {
                    pattern.steps[i].note = 36 + rng.nextInt(25);
                    pattern.steps[i].octave = rng.nextInt(3) - 1;
                    pattern.steps[i].accent = rng.nextFloat() < 0.25f;
                    pattern.steps[i].slide = rng.nextFloat() < 0.2f;
                    pattern.steps[i].active = rng.nextFloat() < 0.8f;
                }
            }
        }
        updateStepDisplay();
    };
    addAndMakeVisible(modifyUndoButton_);

    // Pattern Clear: clears all steps
    patternClearButton_.onClick = [this]() {
        auto& pattern = processor_.getSequencer().getCurrentPatternRef();
        for (int i = 0; i < 16; ++i) {
            pattern.steps[i].active = false;
            pattern.steps[i].note = 60;
            pattern.steps[i].octave = 0;
            pattern.steps[i].accent = false;
            pattern.steps[i].slide = false;
        }
        hasGenerateUndo_ = false;
        hasModifyUndo_ = false;
        updateStepDisplay();
    };
    addAndMakeVisible(patternClearButton_);

    // ALL 16 step buttons
    for (int i = 0; i < 16; ++i) {
        auto* btn = new StepButton(i);
        btn->onToggle = [this](int step, bool active) {
            auto& pattern = processor_.getSequencer().getCurrentPatternRef();
            pattern.steps[step].active = active;
        };
        stepButtons_.add(btn);
        addAndMakeVisible(btn);
    }

    startTimerHz(30);
    updateStepDisplay();
}

BottomPanel::~BottomPanel() { stopTimer(); }

void BottomPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    (void)bounds;

    // Row 3 panels (no Transport here - moved to Row 4)
    TB303LookAndFeel::paintSectionPanel(g, { 10, 5, 760, 220 }, "Keyboard", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 775, 5, 145, 220 }, "Octave", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 925, 5, 175, 220 }, "Articulation", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 1105, 5, 390, 220 }, "Randomize", TB303Colors::pink());

    // Note name labels above keyboard
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    int kbX = 140;
    int kbW = 610;
    float whiteKeyW = static_cast<float>(kbW) / 8.0f;
    int whiteIdx = 0;
    for (int i = 0; i < 13; ++i) {
        bool isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10);
        if (!isBlack) {
            float xPos = static_cast<float>(kbX) + static_cast<float>(whiteIdx) * whiteKeyW + whiteKeyW * 0.5f;
            g.drawText(noteNames[i], static_cast<int>(xPos - 10), 22, 20, 12, juce::Justification::centred);
            whiteIdx++;
        }
    }

    // Row 4 panels
    TB303LookAndFeel::paintSectionPanel(g, { 10, 230, 110, 235 }, "Transport", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 125, 230, 1370, 235 }, "Step Sequencer", TB303Colors::pink());

    // PATTERN label and step numbers
    g.setColour(TB303Colors::pink());
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("PATTERN", 135, 250, 80, 16, juce::Justification::centredLeft);

    g.setFont(juce::Font(10.0f, juce::Font::bold));
    int stepStartX = 230;
    int stepW = 75;
    for (int i = 0; i < 16; ++i) {
        g.setColour(TB303Colors::pink());
        g.drawText(juce::String(i + 1), stepStartX + i * stepW, 250, stepW, 14, juce::Justification::centred);
    }
}

void BottomPanel::resized()
{
    // Keyboard [10, 5, 760, 220] - wider now since Transport moved to Row 4
    keyboard_.setBounds(25, 40, 730, 170);

    // Transport moved to Row 4 [10, 230, 110, 235]
    runStopButton_.setBounds(20, 260, 90, 90);
    recordButton_.setBounds(20, 360, 90, 90);

    // Octave [775, 5, 145, 220]
    upButton_.setBounds(790, 40, 115, 85);
    downButton_.setBounds(790, 130, 115, 85);

    // Articulation [925, 5, 175, 220]
    accentButton_.setBounds(940, 40, 70, 80);
    slideButton_.setBounds(1015, 40, 70, 80);

    // Pattern [1105, 5, 390, 220] - "RANDOMIZE" painted as label in paint()
    generateUndoButton_.setBounds(1120, 50, 170, 48);
    modifyUndoButton_.setBounds(1120, 106, 170, 48);
    patternClearButton_.setBounds(1120, 162, 170, 48);

    // Step buttons [125, 230, 1370, 235] - 16 buttons
    int stepStartX = 230;
    int stepW = 75;
    int stepY = 270;
    int stepH = 180;
    for (int i = 0; i < 16; ++i)
        stepButtons_[i]->setBounds(stepStartX + i * stepW, stepY, stepW - 4, stepH);
}

void BottomPanel::timerCallback()
{
    if (processor_.getSequencer().isPlaying())
    {
        int cs = processor_.getSequencer().getCurrentStep();
        for (int i = 0; i < 16; ++i)
            stepButtons_[i]->setCurrent(cs == i);
    }
    else
    {
        for (int i = 0; i < 16; ++i)
            stepButtons_[i]->setCurrent(false);
    }
    runStopButton_.setLEDOn(processor_.getSequencer().isPlaying());
}

void BottomPanel::updateStepDisplay()
{
    auto& pattern = processor_.getSequencer().getCurrentPatternRef();
    for (int i = 0; i < 16; ++i)
        stepButtons_[i]->setActive(pattern.steps[i].active);
}
