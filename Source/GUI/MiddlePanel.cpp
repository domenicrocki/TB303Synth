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
    patternLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::getTextDim());
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
    patchLabel_.setColour(juce::Label::textColourId, TB303LookAndFeel::getTextDim());
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
    TB303LookAndFeel::drawFuturisticPanel(g, bounds, TB303LookAndFeel::getNeonPurple());

    // PLAY MODE label
    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("PLAY MODE", 280, 6, 80, 10, juce::Justification::centred);

    // LIST label
    g.drawText("LIST", 430, 6, 30, 10, juce::Justification::centredLeft);

    // Separator
    float sepX = bounds.getWidth() - 240.0f;
    TB303LookAndFeel::drawGlowLine(g, sepX, 8.0f, sepX, bounds.getHeight() - 8.0f,
                                     TB303LookAndFeel::getNeonPurple());

    // TB-303 branding (futuristic)
    int brandX = static_cast<int>(bounds.getWidth()) - 230;

    g.setColour(TB303LookAndFeel::getNeonCyan());
    g.setFont(juce::Font(34.0f, juce::Font::bold));
    g.drawText("TB-303", brandX, 8, 200, 40, juce::Justification::centredRight);

    // Glow
    g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.06f));
    g.setFont(juce::Font(34.0f, juce::Font::bold));
    g.drawText("TB-303", brandX - 1, 7, 200, 40, juce::Justification::centredRight);

    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(11.0f));
    g.drawText("Computer Controlled", brandX, 46, 200, 16, juce::Justification::centredRight);

    // Horizontal accent line
    TB303LookAndFeel::drawGlowLine(g, static_cast<float>(brandX + 60), 46.0f,
                                     bounds.getWidth() - 14.0f, 46.0f, TB303LookAndFeel::getNeonCyan());

    // Level meter (futuristic)
    auto meterBounds = juce::Rectangle<float>(470.0f, 92.0f, 150.0f, 4.0f);
    g.setColour(TB303LookAndFeel::getBgLight());
    g.fillRoundedRectangle(meterBounds, 2.0f);
    // Gradient fill
    juce::ColourGradient meterGrad(TB303LookAndFeel::getNeonCyan(), meterBounds.getX(), meterBounds.getCentreY(),
                                     TB303LookAndFeel::getNeonPink(), meterBounds.getRight(), meterBounds.getCentreY(), false);
    g.setGradientFill(meterGrad);
    g.fillRoundedRectangle(meterBounds.withWidth(meterBounds.getWidth() * 0.7f), 2.0f);
}

void MiddlePanel::resized()
{
    int w = getWidth();
    int y = 10;

    optionButton_.setBounds(10, y, 50, 20);
    helpButton_.setBounds(62, y, 40, 20);
    aboutButton_.setBounds(104, y, 48, 20);

    shuffleKnob_.setBounds(10, y + 26, 68, 68);
    scaleKnob_.setBounds(82, y + 26, 68, 68);
    tempoKnob_.setBounds(160, y + 12, 68, 72);

    playModeBox_.setBounds(268, y + 18, 110, 44);

    patternLabel_.setBounds(430, y + 8, 60, 12);
    patternList_.setBounds(470, y + 8, 170, 22);
    patternDown_.setBounds(644, y + 8, 22, 22);
    patternUp_.setBounds(668, y + 8, 22, 22);
    patternWrite_.setBounds(694, y + 8, 50, 22);

    patchLabel_.setBounds(430, y + 38, 60, 12);
    patchList_.setBounds(470, y + 38, 170, 22);
    patchDown_.setBounds(644, y + 38, 22, 22);
    patchUp_.setBounds(668, y + 38, 22, 22);
    patchWrite_.setBounds(694, y + 38, 50, 22);

    int bankX = w - 185;
    int bankW = 34;
    int bankH = 26;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * (bankW + 4), y + 66, bankW, bankH);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * (bankW + 4), y + 66 + bankH + 3, bankW, bankH);
}
