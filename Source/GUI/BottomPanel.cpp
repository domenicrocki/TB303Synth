#include "BottomPanel.h"
#include "TB303LookAndFeel.h"

BottomPanel::BottomPanel(TB303AudioProcessor& processor)
    : processor_(processor)
{
    // Transport
    runStopButton_.setToggleMode(true);
    runStopButton_.setAccentColor(TB303Colors::pink());
    runStopButton_.onClick = [this](bool on) {
        if (on) {
            processor_.getSequencer().start();
        } else {
            processor_.getSequencer().stop();
            processor_.releaseNote(); // silence any held note
        }
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
        } else {
            // Live play
            processor_.triggerNote(midiNote, accentButton_.isLEDOn(), slideButton_.isLEDOn());
        }
    };

    keyboard_.onNoteOff = [this](int) {
        if (!editMode_) {
            processor_.releaseNote();
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

    // Sequencer controls (moved from TopPanel)
    scaleBox_.addItemList(juce::StringArray{ "1/16", "1/16T", "1/32" }, 1);
    addAndMakeVisible(scaleBox_);
    scaleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "scale", scaleBox_);

    playModeBox_.addItemList(juce::StringArray{ "FWD", "REV", "FWD&REV", "INVERT", "RANDOM" }, 1);
    addAndMakeVisible(playModeBox_);
    playModeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "playMode", playModeBox_);

    for (int i = 0; i < 8; ++i)
        patternList_.addItem("Pattern " + juce::String(i + 1), i + 1);
    patternList_.setSelectedId(1);
    addAndMakeVisible(patternList_);
    patternList_.onChange = [this]() {
        processor_.getSequencer().setCurrentPattern(patternList_.getSelectedId() - 1);
        updateStepDisplay();
    };

    updatePatchList();
    addAndMakeVisible(patchList_);
    patchList_.onChange = [this]() {
        int sel = patchList_.getSelectedId() - 1;
        if (sel >= 0) processor_.loadPatch(sel);
    };

    for (int i = 0; i < 8; ++i) {
        auto* btn = new juce::TextButton(juce::String(i + 1));
        btn->onClick = [this, i]() {
            processor_.getSequencer().setCurrentBank(i);
            for (int j = 0; j < 8; ++j)
                bankButtons_[j]->setToggleState(j == i, juce::dontSendNotification);
            updateStepDisplay();
        };
        btn->setClickingTogglesState(false);
        bankButtons_.add(btn);
        addAndMakeVisible(btn);
    }
    if (bankButtons_.size() > 0)
        bankButtons_[0]->setToggleState(true, juce::dontSendNotification);

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
    int w = getWidth();
    int h = getHeight();
    int row1H = 160;

    // Row 1 panels
    TB303LookAndFeel::paintSectionPanel(g, { 4, 4, 195, row1H - 6 }, "Sequencer", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 203, 4, 350, row1H - 6 }, "Keyboard", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 557, 4, 105, row1H - 6 }, "Octave", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 666, 4, 125, row1H - 6 }, "Articulation", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 795, 4, w - 799, row1H - 6 }, "Randomize", TB303Colors::pink());

    // Note labels
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" };
    int kbX = 212;
    int kbW = 330;
    float whiteKeyW = static_cast<float>(kbW) / 8.0f;
    int whiteIdx = 0;
    for (int i = 0; i < 13; ++i) {
        bool isBlack = (i == 1 || i == 3 || i == 6 || i == 8 || i == 10);
        if (!isBlack) {
            float xPos = static_cast<float>(kbX) + static_cast<float>(whiteIdx) * whiteKeyW + whiteKeyW * 0.5f;
            g.drawText(noteNames[i], static_cast<int>(xPos - 8), 18, 16, 10, juce::Justification::centred);
            whiteIdx++;
        }
    }

    // Row 2 panels
    TB303LookAndFeel::paintSectionPanel(g, { 4, row1H + 2, 80, h - row1H - 6 }, "Transport", TB303Colors::pink());
    TB303LookAndFeel::paintSectionPanel(g, { 88, row1H + 2, w - 92, h - row1H - 6 }, "Step Sequencer", TB303Colors::pink());

    // Step numbers
    g.setColour(TB303Colors::pink());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("PATTERN", 96, row1H + 14, 50, 10, juce::Justification::centredLeft);

    int stepStartX = 150;
    int stepW = (w - stepStartX - 10) / 16;
    for (int i = 0; i < 16; ++i) {
        g.drawText(juce::String(i + 1), stepStartX + i * stepW, row1H + 14, stepW, 10, juce::Justification::centred);
    }
}

void BottomPanel::resized()
{
    int w = getWidth();
    int h = getHeight();
    int row1H = 160;

    // Sequencer [4, 4, 195, row1H-6]
    scaleBox_.setBounds(12, 22, 80, 18);
    playModeBox_.setBounds(96, 22, 95, 18);
    patternList_.setBounds(12, 46, 95, 18);
    patchList_.setBounds(12, 68, 95, 18);
    int bankX = 112;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * 24, 46, 20, 16);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * 24, 66, 20, 16);

    // Keyboard [203, 4, 350, row1H-6]
    keyboard_.setBounds(212, 30, 330, 120);

    // Octave [557, 4, 105, row1H-6]
    upButton_.setBounds(565, 24, 88, 55);
    downButton_.setBounds(565, 84, 88, 55);

    // Articulation [666, 4, 125, row1H-6]
    accentButton_.setBounds(674, 24, 52, 60);
    slideButton_.setBounds(730, 24, 52, 60);

    // Randomize [795, 4, ...]
    generateUndoButton_.setBounds(805, 24, w - 815, 36);
    modifyUndoButton_.setBounds(805, 65, w - 815, 36);
    patternClearButton_.setBounds(805, 106, w - 815, 36);

    // Row 2 - Transport [4, row1H+2, 80, ...]
    runStopButton_.setBounds(10, row1H + 16, 68, 70);
    recordButton_.setBounds(10, row1H + 92, 68, 70);

    // Step buttons [88, row1H+2, ...]
    int stepStartX = 150;
    int stepW = (w - stepStartX - 10) / 16;
    int stepY = row1H + 28;
    int stepH = h - stepY - 8;
    for (int i = 0; i < 16; ++i)
        stepButtons_[i]->setBounds(stepStartX + i * stepW, stepY, stepW - 3, stepH);
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

void BottomPanel::updatePatchList()
{
    patchList_.clear();
    auto names = processor_.getPresetManager().getPatchNames();
    for (int i = 0; i < names.size(); ++i)
        patchList_.addItem(names[i], i + 1);
    if (names.size() > 0)
        patchList_.setSelectedId(1, juce::dontSendNotification);
}

void BottomPanel::updateStepDisplay()
{
    auto& pattern = processor_.getSequencer().getCurrentPatternRef();
    for (int i = 0; i < 16; ++i)
        stepButtons_[i]->setActive(pattern.steps[i].active);
}
