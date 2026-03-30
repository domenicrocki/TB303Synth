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
    patternLabel_.setFont(juce::Font(8.0f, juce::Font::bold));
    patternLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::getTextDark());
    addAndMakeVisible(patternLabel_);

    for (int i = 0; i < 8; ++i)
        patternList_.addItem("Preset " + juce::String(i + 1).paddedLeft('0', 3), i + 1);
    patternList_.setSelectedId(1);
    addAndMakeVisible(patternList_);
    addAndMakeVisible(patternUp_);
    addAndMakeVisible(patternDown_);
    addAndMakeVisible(patternWrite_);

    patternUp_.onClick = [this]() {
        int next = patternList_.getSelectedId() + 1;
        if (next <= 8) { patternList_.setSelectedId(next); processor_.getSequencer().setCurrentPattern(next - 1); }
    };
    patternDown_.onClick = [this]() {
        int prev = patternList_.getSelectedId() - 1;
        if (prev >= 1) { patternList_.setSelectedId(prev); processor_.getSequencer().setCurrentPattern(prev - 1); }
    };
    patternList_.onChange = [this]() {
        processor_.getSequencer().setCurrentPattern(patternList_.getSelectedId() - 1);
    };

    // Patch list
    patchLabel_.setFont(juce::Font(8.0f, juce::Font::bold));
    patchLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::getTextDark());
    addAndMakeVisible(patchLabel_);

    updatePatchList();
    addAndMakeVisible(patchList_);
    addAndMakeVisible(patchUp_);
    addAndMakeVisible(patchDown_);
    addAndMakeVisible(patchWrite_);

    patchUp_.onClick = [this]() {
        int next = patchList_.getSelectedId() + 1;
        if (next <= processor_.getPresetManager().getNumPatches()) {
            patchList_.setSelectedId(next); processor_.loadPatch(next - 1);
        }
    };
    patchDown_.onClick = [this]() {
        int prev = patchList_.getSelectedId() - 1;
        if (prev >= 1) { patchList_.setSelectedId(prev); processor_.loadPatch(prev - 1); }
    };
    patchList_.onChange = [this]() {
        int sel = patchList_.getSelectedId() - 1;
        if (sel >= 0) processor_.loadPatch(sel);
    };

    patchWrite_.onClick = [this]() {
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
        btn->onClick = [this, i]() {
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
    TB303LookAndFeel::drawSilverPanel(g, bounds, true);

    g.setColour(TB303LookAndFeel::getTextDark());

    // Scale knob markings
    g.setFont(juce::Font(7.0f));
    int scaleX = 185;
    g.drawText("1/16T", scaleX - 5, 78, 30, 10, juce::Justification::centred);
    g.drawText("1/16", scaleX + 22, 64, 25, 10, juce::Justification::centred);
    g.drawText("1/32", scaleX + 38, 78, 25, 10, juce::Justification::centred);

    // PLAY MODE label
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("PLAY MODE", 290, 3, 80, 10, juce::Justification::centred);

    // Play mode rotary indicators
    g.setFont(juce::Font(7.0f));
    int pmX = 300;
    g.drawText("FORWARD", pmX - 35, 20, 55, 10, juce::Justification::centredRight);
    g.drawText("REVERSE", pmX + 55, 28, 55, 10, juce::Justification::centredLeft);
    g.drawText("FWD&REV", pmX + 55, 48, 55, 10, juce::Justification::centredLeft);
    g.drawText("INVERT", pmX + 35, 68, 55, 10, juce::Justification::centredLeft);
    g.drawText("RANDOM", pmX - 35, 68, 55, 10, juce::Justification::centredRight);

    // LIST label
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("LIST", 430, 3, 30, 10, juce::Justification::centredLeft);

    // Separator before TB-303 branding
    float sepX = bounds.getWidth() - 240.0f;
    g.setColour(juce::Colour(0xFFA0A0A8));
    g.drawLine(sepX, 5.0f, sepX, bounds.getHeight() - 5.0f, 1.0f);

    // TB-303 branding
    g.setColour(TB303LookAndFeel::getTextDark());
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    int brandX = static_cast<int>(bounds.getWidth()) - 230;
    g.drawText("TB-303", brandX, 8, 200, 38, juce::Justification::centredRight);

    g.setFont(juce::Font(11.0f));
    g.drawText("Computer Controlled", brandX, 46, 200, 16, juce::Justification::centredRight);

    // Horizontal line under TB-303
    g.setColour(juce::Colour(0xFF808088));
    g.drawLine(static_cast<float>(brandX + 50), 44.0f, bounds.getWidth() - 12.0f, 44.0f, 1.0f);

    // Level meter bar (simple visual)
    auto meterBounds = juce::Rectangle<float>(470.0f, 95.0f, 150.0f, 6.0f);
    g.setColour(juce::Colour(0xFF404048));
    g.fillRoundedRectangle(meterBounds, 2.0f);
    g.setColour(juce::Colour(0xFFE04040));
    g.fillRoundedRectangle(meterBounds.withWidth(meterBounds.getWidth() * 0.7f), 2.0f);
}

void MiddlePanel::resized()
{
    int w = getWidth();
    int y = 8;

    // Option/Help/About buttons
    optionButton_.setBounds(10, y, 50, 20);
    helpButton_.setBounds(62, y, 40, 20);
    aboutButton_.setBounds(104, y, 48, 20);

    // Shuffle knob
    shuffleKnob_.setBounds(10, y + 26, 70, 72);

    // Scale knob
    scaleKnob_.setBounds(85, y + 26, 70, 72);

    // Tempo knob
    tempoKnob_.setBounds(165, y + 10, 70, 72);

    // Play mode (as combo box styled like rotary)
    playModeBox_.setBounds(275, y + 18, 100, 48);

    // Pattern section
    patternLabel_.setBounds(430, y + 8, 60, 12);
    patternList_.setBounds(470, y + 8, 170, 22);
    patternDown_.setBounds(644, y + 8, 22, 22);
    patternUp_.setBounds(668, y + 8, 22, 22);
    patternWrite_.setBounds(694, y + 8, 50, 22);

    // Patch section
    patchLabel_.setBounds(430, y + 40, 60, 12);
    patchList_.setBounds(470, y + 40, 170, 22);
    patchDown_.setBounds(644, y + 40, 22, 22);
    patchUp_.setBounds(668, y + 40, 22, 22);
    patchWrite_.setBounds(694, y + 40, 50, 22);

    // Bank buttons (2 rows of 4) - right of TB-303 label
    int bankX = w - 185;
    int bankW = 34;
    int bankH = 28;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * (bankW + 4), y + 68, bankW, bankH);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * (bankW + 4), y + 68 + bankH + 4, bankW, bankH - 2);
}
