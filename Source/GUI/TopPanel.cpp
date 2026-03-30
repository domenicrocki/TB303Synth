#include "TopPanel.h"
#include "TB303LookAndFeel.h"

TopPanel::TopPanel(TB303AudioProcessor& processor)
    : processor_(processor),
      apvts_(processor.getAPVTS()),
      tuningKnob_("TUNING", "tuning", processor.getAPVTS()),
      cutoffKnob_("CUT OFF FREQ", "cutoff", processor.getAPVTS()),
      resonanceKnob_("RESONANCE", "resonance", processor.getAPVTS()),
      envModKnob_("ENV MOD", "envMod", processor.getAPVTS()),
      decayKnob_("DECAY", "decay", processor.getAPVTS()),
      accentKnob_("ACCENT", "accent", processor.getAPVTS()),
      tempoKnob_("TEMPO", "tempo", processor.getAPVTS()),
      driveDepthKnob_("DRIVE", "driveDepth", processor.getAPVTS()),
      driveToneKnob_("TONE", "driveTone", processor.getAPVTS()),
      driveLevelKnob_("LEVEL", "driveLevel", processor.getAPVTS()),
      delayTimeKnob_("TIME", "delayTime", processor.getAPVTS()),
      delayLevelKnob_("LEVEL", "delayLevel", processor.getAPVTS()),
      masterTuneKnob_("MASTER\nTUNE", "masterTune", processor.getAPVTS()),
      shuffleKnob_("SHUFFLE", "shuffle", processor.getAPVTS())
{
    // Set arc colors per section
    tuningKnob_.setArcColor(TB303Colors::cyan());
    cutoffKnob_.setArcColor(TB303Colors::blue());
    resonanceKnob_.setArcColor(TB303Colors::blue());
    envModKnob_.setArcColor(TB303Colors::purple());
    decayKnob_.setArcColor(TB303Colors::purple());
    accentKnob_.setArcColor(TB303Colors::purple());
    tempoKnob_.setArcColor(TB303Colors::cyan());
    driveDepthKnob_.setArcColor(TB303Colors::orange());
    driveToneKnob_.setArcColor(TB303Colors::orange());
    driveLevelKnob_.setArcColor(TB303Colors::orange());
    delayTimeKnob_.setArcColor(TB303Colors::orange());
    delayLevelKnob_.setArcColor(TB303Colors::orange());
    masterTuneKnob_.setArcColor(TB303Colors::green());
    shuffleKnob_.setArcColor(TB303Colors::green());

    // VCO waveform buttons
    sawButton_.setButtonText("SAW");
    sawButton_.setToggleState(true, juce::dontSendNotification);
    sawButton_.setClickingTogglesState(false);
    sawButton_.onClick = [this]() {
        currentWaveform_ = 0;
        apvts_.getParameter("waveform")->setValueNotifyingHost(0.0f);
        sawButton_.setToggleState(true, juce::dontSendNotification);
        sqrButton_.setToggleState(false, juce::dontSendNotification);
    };
    addAndMakeVisible(sawButton_);

    sqrButton_.setButtonText("SQR");
    sqrButton_.setToggleState(false, juce::dontSendNotification);
    sqrButton_.setClickingTogglesState(false);
    sqrButton_.onClick = [this]() {
        currentWaveform_ = 1;
        apvts_.getParameter("waveform")->setValueNotifyingHost(1.0f);
        sawButton_.setToggleState(false, juce::dontSendNotification);
        sqrButton_.setToggleState(true, juce::dontSendNotification);
    };
    addAndMakeVisible(sqrButton_);

    addAndMakeVisible(tuningKnob_);
    addAndMakeVisible(cutoffKnob_);
    addAndMakeVisible(resonanceKnob_);
    addAndMakeVisible(envModKnob_);
    addAndMakeVisible(decayKnob_);
    addAndMakeVisible(accentKnob_);

    // Volume
    volumeSlider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider_.setPopupDisplayEnabled(true, true, this);
    volumeSlider_.getProperties().set("arcColor", (juce::int64)TB303Colors::green().getARGB());
    addAndMakeVisible(volumeSlider_);
    volumeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts_, "volume", volumeSlider_);

    // Clock / Tempo
    addAndMakeVisible(tempoKnob_);

    // Overdrive
    driveTypeBox_.addItemList(juce::StringArray{ "Soft", "Hard", "Tube" }, 1);
    addAndMakeVisible(driveTypeBox_);
    driveTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts_, "driveType", driveTypeBox_);
    addAndMakeVisible(driveDepthKnob_);
    addAndMakeVisible(driveToneKnob_);
    addAndMakeVisible(driveLevelKnob_);

    // Delay
    delayTypeBox_.addItemList(juce::StringArray{ "Digital", "Tape", "PingPong" }, 1);
    addAndMakeVisible(delayTypeBox_);
    delayTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts_, "delayType", delayTypeBox_);
    addAndMakeVisible(delayTimeKnob_);
    addAndMakeVisible(delayLevelKnob_);

    tempoSyncButton_.setButtonText("SYNC");
    addAndMakeVisible(tempoSyncButton_);
    tempoSyncAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts_, "tempoSync", tempoSyncButton_);

    // Master
    addAndMakeVisible(masterTuneKnob_);
    addAndMakeVisible(shuffleKnob_);

    // Sequencer section
    scaleBox_.addItemList(juce::StringArray{ "1/16", "1/16T", "1/32" }, 1);
    addAndMakeVisible(scaleBox_);
    scaleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts_, "scale", scaleBox_);

    playModeBox_.addItemList(juce::StringArray{ "FWD", "REV", "FWD&REV", "INVERT", "RANDOM" }, 1);
    addAndMakeVisible(playModeBox_);
    playModeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts_, "playMode", playModeBox_);

    // Pattern list
    for (int i = 0; i < 8; ++i)
        patternList_.addItem("Pattern " + juce::String(i + 1), i + 1);
    patternList_.setSelectedId(1);
    addAndMakeVisible(patternList_);
    patternList_.onChange = [this]() {
        processor_.getSequencer().setCurrentPattern(patternList_.getSelectedId() - 1);
    };

    // Patch list
    updatePatchList();
    addAndMakeVisible(patchList_);
    patchList_.onChange = [this]() {
        int sel = patchList_.getSelectedId() - 1;
        if (sel >= 0) processor_.loadPatch(sel);
    };

    // Bank buttons
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

    startTimerHz(15);
}

TopPanel::~TopPanel() { stopTimer(); }

void TopPanel::updatePatchList()
{
    patchList_.clear();
    auto names = processor_.getPresetManager().getPatchNames();
    for (int i = 0; i < names.size(); ++i)
        patchList_.addItem(names[i], i + 1);
    if (names.size() > 0)
        patchList_.setSelectedId(1, juce::dontSendNotification);
}

void TopPanel::timerCallback()
{
    repaint(); // update BPM display
}

void TopPanel::paint(juce::Graphics& g)
{
    // Row 1 panels
    TB303LookAndFeel::paintSectionPanel(g, { 5, 5, 215, 150 }, "Oscillator (VCO)", TB303Colors::cyan());
    TB303LookAndFeel::paintSectionPanel(g, { 225, 5, 295, 150 }, "Filter (VCF)", TB303Colors::blue());
    TB303LookAndFeel::paintSectionPanel(g, { 525, 5, 305, 150 }, "Filter Envelope", TB303Colors::purple());
    TB303LookAndFeel::paintSectionPanel(g, { 835, 5, 185, 150 }, "Mixer (VCA)", TB303Colors::green());
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("VOLUME", 835, 130, 185, 14, juce::Justification::centred);

    // Branding area
    TB303LookAndFeel::paintSectionPanel(g, { 1025, 5, 470, 150 }, "", TB303Colors::cyan());
    g.setColour(TB303Colors::cyan());
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("Rocki", 1040, 20, 200, 45, juce::Justification::centredLeft);
    g.setColour(TB303Colors::textBright());
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("TB-303", 1040, 65, 200, 35, juce::Justification::centredLeft);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(12.0f));
    g.drawText("Computer Controlled", 1040, 100, 200, 18, juce::Justification::centredLeft);

    // Row 2 panels
    TB303LookAndFeel::paintSectionPanel(g, { 5, 160, 215, 145 }, "Clock", TB303Colors::cyan());
    TB303LookAndFeel::paintSectionPanel(g, { 225, 160, 295, 145 }, "Overdrive", TB303Colors::orange());
    TB303LookAndFeel::paintSectionPanel(g, { 525, 160, 305, 145 }, "Delay", TB303Colors::orange());
    TB303LookAndFeel::paintSectionPanel(g, { 835, 160, 185, 145 }, "Master", TB303Colors::green());
    TB303LookAndFeel::paintSectionPanel(g, { 1025, 160, 470, 145 }, "Sequencer", TB303Colors::pink());

    // BPM display in Clock section
    float bpm = *apvts_.getRawParameterValue("tempo");
    g.setColour(TB303Colors::cyan());
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText(juce::String(bpm, 1), 15, 180, 195, 50, juce::Justification::centred);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(11.0f));
    g.drawText("BPM", 15, 228, 195, 16, juce::Justification::centred);
}

void TopPanel::resized()
{
    // --- Row 1 ---
    // VCO section [5, 5, 215, 150]
    sawButton_.setBounds(20, 28, 55, 24);
    sqrButton_.setBounds(80, 28, 55, 24);
    tuningKnob_.setBounds(95, 28, 110, 115);

    // VCF section [225, 5, 295, 150]
    cutoffKnob_.setBounds(240, 28, 130, 115);
    resonanceKnob_.setBounds(375, 28, 130, 115);

    // Filter Envelope [525, 5, 305, 150]
    envModKnob_.setBounds(535, 28, 90, 115);
    decayKnob_.setBounds(630, 28, 90, 115);
    accentKnob_.setBounds(725, 28, 90, 115);

    // VCA [835, 5, 185, 150]
    volumeSlider_.setBounds(865, 30, 120, 115);

    // --- Row 2 ---
    // Clock [5, 160, 215, 145] - BPM painted, tempo knob
    tempoKnob_.setBounds(55, 230, 120, 70);

    // Overdrive [225, 160, 295, 145]
    driveTypeBox_.setBounds(240, 180, 120, 22);
    driveDepthKnob_.setBounds(238, 206, 90, 90);
    driveToneKnob_.setBounds(330, 206, 90, 90);
    driveLevelKnob_.setBounds(422, 206, 90, 90);

    // Delay [525, 160, 305, 145]
    delayTypeBox_.setBounds(540, 180, 120, 22);
    delayTimeKnob_.setBounds(540, 206, 90, 90);
    delayLevelKnob_.setBounds(635, 206, 90, 90);
    tempoSyncButton_.setBounds(735, 210, 80, 24);

    // Master [835, 160, 185, 145]
    masterTuneKnob_.setBounds(845, 180, 80, 90);
    shuffleKnob_.setBounds(930, 180, 80, 90);

    // Sequencer [1025, 160, 470, 145]
    scaleBox_.setBounds(1040, 180, 90, 22);
    playModeBox_.setBounds(1140, 180, 100, 22);

    patternList_.setBounds(1040, 210, 150, 22);
    patchList_.setBounds(1040, 238, 150, 22);

    // Bank buttons in sequencer section
    int bankX = 1200;
    int bankW = 32;
    int bankH = 22;
    for (int i = 0; i < 4; ++i)
        bankButtons_[i]->setBounds(bankX + i * (bankW + 4), 210, bankW, bankH);
    for (int i = 4; i < 8; ++i)
        bankButtons_[i]->setBounds(bankX + (i - 4) * (bankW + 4), 238, bankW, bankH);
}
