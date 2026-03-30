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

    // Shuffle
    addAndMakeVisible(shuffleKnob_);

    startTimerHz(15);
}

TopPanel::~TopPanel() { stopTimer(); }

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
    TB303LookAndFeel::paintSectionPanel(g, { 1025, 5, 470, 300 }, "", TB303Colors::cyan());
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
    TB303LookAndFeel::paintSectionPanel(g, { 835, 160, 185, 145 }, "Shuffle", TB303Colors::green());
    // (Sequencer section moved to BottomPanel)

    // BPM display in Clock section
    float bpm = *apvts_.getRawParameterValue("tempo");
    g.setColour(TB303Colors::cyan());
    g.setFont(juce::Font(38.0f, juce::Font::bold));
    g.drawText(juce::String(bpm, 1), 15, 178, 195, 44, juce::Justification::centred);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(11.0f));
    g.drawText("BPM", 15, 218, 195, 14, juce::Justification::centred);
}

void TopPanel::resized()
{
    // --- Row 1 ---
    // VCO section [5, 5, 215, 150]
    sawButton_.setBounds(20, 28, 70, 24);
    sqrButton_.setBounds(20, 56, 70, 24);
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
    // Clock [5, 160, 215, 145] - BPM painted, tempo knob below
    tempoKnob_.setBounds(40, 232, 140, 68);

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

    // Shuffle [835, 160, 185, 145] - centered
    shuffleKnob_.setBounds(870, 180, 120, 110);

    // (Sequencer controls in BottomPanel)
}
