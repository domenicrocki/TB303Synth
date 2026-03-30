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

    playModeBox_.addItemList(juce::StringArray{ "FORWARD", "REVERSE", "FWD&REV", "INVERT", "RANDOM" }, 1);
    addAndMakeVisible(playModeBox_);
    playModeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getAPVTS(), "playMode", playModeBox_);

    patternLabel_.setFont(juce::Font(8.0f, juce::Font::bold));
    patternLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::textDk());
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

    patchLabel_.setFont(juce::Font(8.0f, juce::Font::bold));
    patchLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::textDk());
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

    for (int i = 0; i < 8; ++i) {
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
    TB303LookAndFeel::paintSilverBg(g, bounds, true);

    // PLAY MODE label
    g.setColour(TB303LookAndFeel::textDk());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("PLAY MODE", 430, 6, 80, 10, juce::Justification::centred);

    // Play mode position labels around the combo box area
    g.setFont(juce::Font(7.0f, juce::Font::bold));
    g.setColour(TB303LookAndFeel::textDk().withAlpha(0.7f));
    g.drawText("FORWARD", 430, 76, 80, 10, juce::Justification::centred);
    g.drawText("REVERSE", 510, 55, 50, 10, juce::Justification::centredLeft);
    g.drawText("FWD&REV", 510, 35, 50, 10, juce::Justification::centredLeft);
    g.drawText("INVERT", 390, 35, 50, 10, juce::Justification::centredRight);
    g.drawText("RANDOM", 390, 55, 50, 10, juce::Justification::centredRight);

    // LIST label
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(TB303LookAndFeel::textDk());
    g.drawText("LIST", 580, 6, 30, 10, juce::Justification::centredLeft);

    // Scale markings near scale knob
    g.setFont(juce::Font(7.0f, juce::Font::bold));
    g.setColour(TB303LookAndFeel::textDk().withAlpha(0.7f));
    g.drawText("1/16T", 330, 92, 30, 10, juce::Justification::centred);
    g.drawText("1/16", 355, 76, 25, 10, juce::Justification::centred);
    g.drawText("1/32", 365, 56, 25, 10, juce::Justification::centred);

    // Separator before TB-303 branding
    float sepX = bounds.getWidth() - 240.0f;
    g.setColour(juce::Colour(0xFF909098));
    g.drawLine(sepX, 8.0f, sepX, bounds.getHeight() - 8.0f, 1.0f);

    // TB-303 branding (silver metallic style)
    int brandX = static_cast<int>(bounds.getWidth()) - 230;

    g.setColour(TB303LookAndFeel::textDk());
    g.setFont(juce::Font(34.0f, juce::Font::bold));
    g.drawText("TB-303", brandX, 8, 200, 40, juce::Justification::centredRight);

    g.setColour(TB303LookAndFeel::textDk().withAlpha(0.7f));
    g.setFont(juce::Font(11.0f));
    g.drawText("Computer Controlled", brandX, 46, 200, 16, juce::Justification::centredRight);

    // Horizontal accent line
    g.setColour(juce::Colour(0xFF909098));
    g.drawLine(static_cast<float>(brandX + 60), 66.0f,
               bounds.getWidth() - 14.0f, 66.0f, 1.0f);

    // Level meter bar (red/white)
    auto meterBounds = juce::Rectangle<float>(620.0f, 92.0f, 150.0f, 6.0f);
    g.setColour(juce::Colour(0xFFD0D0D4));
    g.fillRoundedRectangle(meterBounds, 2.0f);
    g.setColour(juce::Colour(0xFF909098));
    g.drawRoundedRectangle(meterBounds, 2.0f, 0.5f);
    // Fill portion
    juce::ColourGradient meterGrad(juce::Colour(0xFF40C040), meterBounds.getX(), meterBounds.getCentreY(),
                                     juce::Colour(0xFFE02020), meterBounds.getRight(), meterBounds.getCentreY(), false);
    g.setGradientFill(meterGrad);
    g.fillRoundedRectangle(meterBounds.withWidth(meterBounds.getWidth() * 0.7f), 2.0f);
}

void MiddlePanel::resized()
{
    int y = 10;

    // Top-left: OPTION, HELP, ABOUT buttons
    optionButton_.setBounds(10, y, 50, 20);
    helpButton_.setBounds(62, y, 40, 20);
    aboutButton_.setBounds(104, y, 48, 20);

    // Bank buttons below: 2 rows of 4
    int bankX = 10;
    int bankW = 34;
    int bankH = 22;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * (bankW + 4), y + 26, bankW, bankH);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * (bankW + 4), y + 26 + bankH + 3, bankW, bankH);

    // SHUFFLE knob
    shuffleKnob_.setBounds(168, y + 10, 72, 80);

    // SCALE knob
    scaleKnob_.setBounds(248, y + 10, 72, 80);

    // TEMPO knob
    tempoKnob_.setBounds(328, y + 10, 72, 80);

    // PLAY MODE combo box
    playModeBox_.setBounds(438, y + 18, 100, 44);

    // Pattern row
    patternLabel_.setBounds(580, y + 8, 55, 12);
    patternList_.setBounds(634, y + 8, 170, 22);
    patternDown_.setBounds(808, y + 8, 22, 22);
    patternUp_.setBounds(832, y + 8, 22, 22);
    patternWrite_.setBounds(858, y + 8, 50, 22);

    // Patch row
    patchLabel_.setBounds(580, y + 38, 55, 12);
    patchList_.setBounds(634, y + 38, 170, 22);
    patchDown_.setBounds(808, y + 38, 22, 22);
    patchUp_.setBounds(832, y + 38, 22, 22);
    patchWrite_.setBounds(858, y + 38, 50, 22);
}
