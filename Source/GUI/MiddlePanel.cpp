#include "MiddlePanel.h"
#include "TB303LookAndFeel.h"

MiddlePanel::MiddlePanel(TB303AudioProcessor& processor)
    : processor_(processor),
      shuffleKnob_("SHUFFLE", "shuffle", processor.getAPVTS()),
      scaleKnob_("SCALE", "scale", processor.getAPVTS()),
      tempoKnob_("TEMPO", "tempo", processor.getAPVTS())
{
    addAndMakeVisible(optionButton_);
    addAndMakeVisible(helpButton_);
    addAndMakeVisible(aboutButton_);
    addAndMakeVisible(shuffleKnob_);
    addAndMakeVisible(scaleKnob_);
    addAndMakeVisible(tempoKnob_);

    // Play mode
    playModeBox_.addItemList(juce::StringArray{ "FORWARD", "REVERSE", "FWD&REV", "INVERT", "RANDOM" }, 1);
    addAndMakeVisible(playModeBox_);
    playModeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "playMode", playModeBox_);

    // Pattern list
    for (int i = 0; i < 8; ++i)
        patternList_.addItem("Pattern " + juce::String(i + 1).paddedLeft('0', 3), i + 1);
    patternList_.setSelectedId(1);
    addAndMakeVisible(patternList_);
    addAndMakeVisible(patternUp_);
    addAndMakeVisible(patternDown_);
    addAndMakeVisible(patternWrite_);

    patternUp_.onClick = [this]()
    {
        int next = patternList_.getSelectedId() + 1;
        if (next <= 8) {
            patternList_.setSelectedId(next);
            processor_.getSequencer().setCurrentPattern(next - 1);
        }
    };
    patternDown_.onClick = [this]()
    {
        int prev = patternList_.getSelectedId() - 1;
        if (prev >= 1) {
            patternList_.setSelectedId(prev);
            processor_.getSequencer().setCurrentPattern(prev - 1);
        }
    };
    patternList_.onChange = [this]()
    {
        int sel = patternList_.getSelectedId() - 1;
        processor_.getSequencer().setCurrentPattern(sel);
    };

    // Patch list
    updatePatchList();
    addAndMakeVisible(patchList_);
    addAndMakeVisible(patchUp_);
    addAndMakeVisible(patchDown_);
    addAndMakeVisible(patchWrite_);

    patchUp_.onClick = [this]()
    {
        int next = patchList_.getSelectedId() + 1;
        if (next <= processor_.getPresetManager().getNumPatches()) {
            patchList_.setSelectedId(next);
            processor_.loadPatch(next - 1);
        }
    };
    patchDown_.onClick = [this]()
    {
        int prev = patchList_.getSelectedId() - 1;
        if (prev >= 1) {
            patchList_.setSelectedId(prev);
            processor_.loadPatch(prev - 1);
        }
    };
    patchList_.onChange = [this]()
    {
        int sel = patchList_.getSelectedId() - 1;
        if (sel >= 0)
            processor_.loadPatch(sel);
    };

    patchWrite_.onClick = [this]()
    {
        // Save current parameters as patch
        auto& apvts = processor_.getAPVTS();
        PatchPreset patch;
        patch.name = "User Patch " + juce::String(patchList_.getSelectedId());
        patch.cutoff = *apvts.getRawParameterValue("cutoff");
        patch.resonance = *apvts.getRawParameterValue("resonance");
        patch.envMod = *apvts.getRawParameterValue("envMod");
        patch.decay = *apvts.getRawParameterValue("decay");
        patch.accent = *apvts.getRawParameterValue("accent");
        patch.tuning = *apvts.getRawParameterValue("tuning");
        patch.waveform = static_cast<int>(*apvts.getRawParameterValue("waveform"));
        patch.driveDepth = *apvts.getRawParameterValue("driveDepth");
        patch.driveType = static_cast<int>(*apvts.getRawParameterValue("driveType"));
        patch.driveTone = *apvts.getRawParameterValue("driveTone");
        patch.delayTime = *apvts.getRawParameterValue("delayTime");
        patch.delayLevel = *apvts.getRawParameterValue("delayLevel");
        patch.delayFeedback = *apvts.getRawParameterValue("delayFeedback");
        patch.delayType = static_cast<int>(*apvts.getRawParameterValue("delayType"));
        patch.tempoSync = *apvts.getRawParameterValue("tempoSync") > 0.5f;

        int idx = patchList_.getSelectedId() - 1;
        if (idx >= 0 && idx < processor_.getPresetManager().getNumPatches())
            processor_.getPresetManager().replacePatch(idx, patch);
        else
            processor_.getPresetManager().addPatch(patch);

        updatePatchList();
    };

    // Bank buttons
    for (int i = 0; i < 8; ++i)
    {
        auto* btn = new juce::TextButton(juce::String(i + 1));
        btn->onClick = [this, i]()
        {
            processor_.getSequencer().setCurrentBank(i);
            for (int j = 0; j < 8; ++j)
                bankButtons_[j]->setToggleState(j == i, juce::dontSendNotification);
        };
        btn->setClickingTogglesState(false);
        bankButtons_.add(btn);
        addAndMakeVisible(btn);
    }
    if (bankButtons_.size() > 0)
        bankButtons_[0]->setToggleState(true, juce::dontSendNotification);
}

MiddlePanel::~MiddlePanel() {}

void MiddlePanel::updatePatchList()
{
    patchList_.clear();
    auto names = processor_.getPresetManager().getPatchNames();
    for (int i = 0; i < names.size(); ++i)
        patchList_.addItem(names[i], i + 1);
    if (names.size() > 0)
        patchList_.setSelectedId(1, juce::dontSendNotification);
}

void MiddlePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Silver background (slightly different shade from top)
    juce::ColourGradient gradient(juce::Colour(0xFFCCCCD0), 0.0f, 0.0f,
                                   juce::Colour(0xFFB8B8BC), 0.0f, bounds.getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // Section labels
    g.setColour(TB303LookAndFeel::getTextColor());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("PLAY MODE", 255, 2, 80, 12, juce::Justification::centred);
    g.drawText("PATTERN", 390, 2, 80, 12, juce::Justification::centred);
    g.drawText("PATCH", 390, 40, 80, 12, juce::Justification::centred);
    g.drawText("LIST", 380, 2, 20, 12, juce::Justification::centred);

    // TB-303 label
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("TB-303", static_cast<int>(bounds.getWidth()) - 220, 5, 180, 35, juce::Justification::centredRight);
    g.setFont(juce::Font(12.0f));
    g.drawText("Computer Controlled", static_cast<int>(bounds.getWidth()) - 220, 38, 180, 16, juce::Justification::centredRight);

    // Separator
    g.setColour(juce::Colour(0xFFA0A0A8));
    g.drawLine(bounds.getWidth() - 230.0f, 5.0f, bounds.getWidth() - 230.0f, bounds.getHeight() - 5.0f, 1.0f);
}

void MiddlePanel::resized()
{
    int y = 8;

    // Option/Help/About
    optionButton_.setBounds(10, y, 50, 22);
    helpButton_.setBounds(62, y, 40, 22);
    aboutButton_.setBounds(104, y, 45, 22);

    // Shuffle, Scale, Tempo knobs
    shuffleKnob_.setBounds(10, y + 28, 60, 55);
    scaleKnob_.setBounds(72, y + 28, 60, 55);
    tempoKnob_.setBounds(144, y + 10, 60, 65);

    // Play mode
    playModeBox_.setBounds(220, y + 14, 110, 22);

    // Pattern list
    patternList_.setBounds(350, y + 12, 150, 22);
    patternDown_.setBounds(502, y + 12, 22, 22);
    patternUp_.setBounds(526, y + 12, 22, 22);
    patternWrite_.setBounds(552, y + 12, 50, 22);

    // Patch list
    patchList_.setBounds(350, y + 48, 150, 22);
    patchDown_.setBounds(502, y + 48, 22, 22);
    patchUp_.setBounds(526, y + 48, 22, 22);
    patchWrite_.setBounds(552, y + 48, 50, 22);

    // Bank buttons (2 rows of 4)
    int bankX = 620;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * 35, y + 5, 30, 25);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * 35, y + 35, 30, 25);
}
