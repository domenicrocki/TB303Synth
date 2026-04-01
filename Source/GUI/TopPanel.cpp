#include "TopPanel.h"
#include "TB303LookAndFeel.h"

TopPanel::TopPanel(TB303AudioProcessor& processor)
    : processor_(processor),
      apvts_(processor.getAPVTS()),
      tuningKnob_("TUNING", "tuning", processor.getAPVTS()),
      cutoffKnob_("CUT OFF", "cutoff", processor.getAPVTS()),
      resonanceKnob_("RESO", "resonance", processor.getAPVTS()),
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
    tuningKnob_.setArcColor(TB303Colors::cyan());
    cutoffKnob_.setArcColor(TB303Colors::blue());
    resonanceKnob_.setArcColor(TB303Colors::blue());
    envModKnob_.setArcColor(TB303Colors::purple());
    decayKnob_.setArcColor(TB303Colors::purple());
    accentKnob_.setArcColor(TB303Colors::purple());
    tempoKnob_.setArcColor(TB303Colors::cyan());
    tempoKnob_.getSlider().setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    tempoKnob_.getSlider().setColour(juce::Slider::textBoxTextColourId, TB303Colors::cyan());
    tempoKnob_.getSlider().setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF14141C));
    tempoKnob_.getSlider().setColour(juce::Slider::textBoxOutlineColourId, TB303Colors::panelBorder());
    driveDepthKnob_.setArcColor(TB303Colors::orange());
    driveToneKnob_.setArcColor(TB303Colors::orange());
    driveLevelKnob_.setArcColor(TB303Colors::orange());
    delayTimeKnob_.setArcColor(TB303Colors::orange());
    delayLevelKnob_.setArcColor(TB303Colors::orange());
    shuffleKnob_.setArcColor(TB303Colors::green());

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

    volumeSlider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider_.setPopupDisplayEnabled(true, true, this);
    volumeSlider_.getProperties().set("arcColor", (juce::int64)TB303Colors::green().getARGB());
    addAndMakeVisible(volumeSlider_);
    volumeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts_, "volume", volumeSlider_);

    addAndMakeVisible(tempoKnob_);

    driveTypeBox_.addItemList(juce::StringArray{ "Soft", "Hard", "Tube" }, 1);
    addAndMakeVisible(driveTypeBox_);
    driveTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts_, "driveType", driveTypeBox_);
    addAndMakeVisible(driveDepthKnob_);
    addAndMakeVisible(driveToneKnob_);
    addAndMakeVisible(driveLevelKnob_);

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

    addAndMakeVisible(shuffleKnob_);

    startTimerHz(15);
}

TopPanel::~TopPanel() { stopTimer(); }

void TopPanel::timerCallback() { repaint(); }

void TopPanel::paint(juce::Graphics& g)
{
    int w = getWidth();
    int rh = getHeight() / 2; // row height

    // Row 1
    TB303LookAndFeel::paintSectionPanel(g, { 4, 4, 155, rh - 6 }, "VCO", TB303Colors::cyan());
    TB303LookAndFeel::paintSectionPanel(g, { 163, 4, 210, rh - 6 }, "Filter (VCF)", TB303Colors::blue());
    TB303LookAndFeel::paintSectionPanel(g, { 377, 4, 225, rh - 6 }, "Envelope", TB303Colors::purple());
    TB303LookAndFeel::paintSectionPanel(g, { 606, 4, 130, rh - 6 }, "VCA", TB303Colors::green());
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("VOLUME", 606, rh - 14, 130, 12, juce::Justification::centred);

    // Branding
    TB303LookAndFeel::paintSectionPanel(g, { 740, 4, w - 744, getHeight() - 8 }, "", TB303Colors::cyan());
    g.setColour(TB303Colors::cyan());
    g.setFont(juce::Font(30.0f, juce::Font::bold));
    g.drawText("Rocki", 755, 14, 160, 30, juce::Justification::centredLeft);
    g.setColour(TB303Colors::textBright());
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("TB-303", 755, 44, 160, 24, juce::Justification::centredLeft);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(10.0f));
    g.drawText("Computer Controlled", 755, 68, 160, 14, juce::Justification::centredLeft);

    // Row 2
    TB303LookAndFeel::paintSectionPanel(g, { 4, rh + 2, 155, rh - 6 }, "Clock", TB303Colors::cyan());
    TB303LookAndFeel::paintSectionPanel(g, { 163, rh + 2, 210, rh - 6 }, "Overdrive", TB303Colors::orange());
    TB303LookAndFeel::paintSectionPanel(g, { 377, rh + 2, 225, rh - 6 }, "Delay", TB303Colors::orange());
    TB303LookAndFeel::paintSectionPanel(g, { 606, rh + 2, 130, rh - 6 }, "Shuffle", TB303Colors::green());

    // BPM display
    float bpm = *apvts_.getRawParameterValue("tempo");
    g.setColour(TB303Colors::cyan());
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText(juce::String(bpm, 1), 10, rh + 16, 145, 30, juce::Justification::centred);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(9.0f));
    g.drawText("BPM", 10, rh + 44, 145, 12, juce::Justification::centred);
}

void TopPanel::resized()
{
    int w = getWidth();
    int rh = getHeight() / 2;
    int kS = 70; // knob size
    int sK = 58; // small knob

    // Row 1 - VCO [4, 4, 155, rh-6]
    sawButton_.setBounds(14, 22, 55, 20);
    sqrButton_.setBounds(14, 44, 55, 20);
    tuningKnob_.setBounds(75, 22, 75, kS);

    // VCF [163, 4, 210, rh-6]
    cutoffKnob_.setBounds(170, 22, 95, kS);
    resonanceKnob_.setBounds(268, 22, 95, kS);

    // Envelope [377, 4, 225, rh-6]
    envModKnob_.setBounds(383, 22, 70, kS);
    decayKnob_.setBounds(456, 22, 70, kS);
    accentKnob_.setBounds(529, 22, 70, kS);

    // VCA [606, 4, 130, rh-6]
    volumeSlider_.setBounds(620, 18, 100, 85);

    // Row 2 - Clock [4, rh+2, 155, rh-6]
    tempoKnob_.setBounds(25, rh + 56, 110, 55);

    // Overdrive [163, rh+2, 210, rh-6]
    driveTypeBox_.setBounds(170, rh + 18, 90, 20);
    driveDepthKnob_.setBounds(170, rh + 42, sK, sK);
    driveToneKnob_.setBounds(230, rh + 42, sK, sK);
    driveLevelKnob_.setBounds(290, rh + 42, sK, sK);

    // Delay [377, rh+2, 225, rh-6]
    delayTypeBox_.setBounds(384, rh + 18, 90, 20);
    delayTimeKnob_.setBounds(384, rh + 42, sK, sK);
    delayLevelKnob_.setBounds(444, rh + 42, sK, sK);
    tempoSyncButton_.setBounds(510, rh + 44, 80, 22);

    // Shuffle [606, rh+2, 130, rh-6]
    shuffleKnob_.setBounds(625, rh + 20, 95, 80);
}
