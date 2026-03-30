#include "TopPanel.h"
#include "TB303LookAndFeel.h"

TopPanel::TopPanel(juce::AudioProcessorValueTreeState& apvts)
    : tuningKnob_("TUNING", "tuning", apvts),
      cutoffKnob_("CUT OFF FREQ", "cutoff", apvts),
      resonanceKnob_("RESONANCE", "resonance", apvts),
      envModKnob_("ENV MOD", "envMod", apvts),
      decayKnob_("DECAY", "decay", apvts),
      accentKnob_("ACCENT", "accent", apvts),
      masterTuneKnob_("MASTER TUNE", "masterTune", apvts),
      driveToneKnob_("TONE", "driveTone", apvts),
      driveDepthKnob_("DEPTH", "driveDepth", apvts),
      delayTimeKnob_("TIME", "delayTime", apvts),
      delayLevelKnob_("LEVEL", "delayLevel", apvts),
      volumeKnob_("VOLUME", "volume", apvts),
      apvts_(apvts)
{
    // Waveform button
    waveformButton_.setButtonText("SAW");
    waveformButton_.setClickingTogglesState(false);
    waveformButton_.onClick = [this]()
    {
        currentWaveform_ = 1 - currentWaveform_;
        apvts_.getParameter("waveform")->setValueNotifyingHost(static_cast<float>(currentWaveform_));
        waveformButton_.setButtonText(currentWaveform_ == 0 ? "SAW" : "SQR");
    };
    addAndMakeVisible(waveformButton_);

    addAndMakeVisible(tuningKnob_);
    addAndMakeVisible(cutoffKnob_);
    addAndMakeVisible(resonanceKnob_);
    addAndMakeVisible(envModKnob_);
    addAndMakeVisible(decayKnob_);
    addAndMakeVisible(accentKnob_);
    addAndMakeVisible(masterTuneKnob_);

    // Drive
    driveTypeBox_.addItemList(juce::StringArray{ "Soft", "Hard", "Tube" }, 1);
    addAndMakeVisible(driveTypeBox_);
    driveTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "driveType", driveTypeBox_);
    addAndMakeVisible(driveToneKnob_);
    addAndMakeVisible(driveDepthKnob_);

    // Delay
    delayTypeBox_.addItemList(juce::StringArray{ "Digital", "Tape", "Ping Pong" }, 1);
    addAndMakeVisible(delayTypeBox_);
    delayTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "delayType", delayTypeBox_);
    addAndMakeVisible(delayTimeKnob_);
    addAndMakeVisible(delayLevelKnob_);

    // Tempo sync
    tempoSyncButton_.setButtonText("TEMPO SYNC");
    addAndMakeVisible(tempoSyncButton_);
    tempoSyncAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "tempoSync", tempoSyncButton_);

    addAndMakeVisible(volumeKnob_);
}

TopPanel::~TopPanel() {}

void TopPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Silver metallic background
    juce::ColourGradient gradient(juce::Colour(0xFFD8D8DC), 0.0f, 0.0f,
                                   juce::Colour(0xFFC0C0C4), 0.0f, bounds.getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // Top border line
    g.setColour(juce::Colour(0xFFE8E8EC));
    g.drawLine(0.0f, 0.0f, bounds.getWidth(), 0.0f, 1.0f);

    // Roland logo area
    g.setColour(TB303LookAndFeel::getTextColor());
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("Roland", 10, 35, 120, 30, juce::Justification::centredLeft);

    // Waveform label
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("WAVEFORM", 15, 5, 80, 12, juce::Justification::centredLeft);

    // Section labels
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("DRIVE TYPE", static_cast<int>(bounds.getWidth()) - 340, 5, 80, 12, juce::Justification::centred);
    g.drawText("DELAY TYPE", static_cast<int>(bounds.getWidth()) - 220, 5, 80, 12, juce::Justification::centred);

    // Separator lines
    g.setColour(juce::Colour(0xFFA0A0A8));
    float sepX1 = bounds.getWidth() - 370.0f;
    g.drawLine(sepX1, 10.0f, sepX1, bounds.getHeight() - 10.0f, 1.0f);
    float sepX2 = bounds.getWidth() - 250.0f;
    g.drawLine(sepX2, 10.0f, sepX2, bounds.getHeight() - 10.0f, 1.0f);
    float sepX3 = bounds.getWidth() - 100.0f;
    g.drawLine(sepX3, 10.0f, sepX3, bounds.getHeight() - 10.0f, 1.0f);
}

void TopPanel::resized()
{
    auto bounds = getLocalBounds();
    int knobSize = 70;
    int smallKnobSize = 55;
    int y = 15;

    // Waveform button
    waveformButton_.setBounds(30, 15, 55, 22);

    // Main synth knobs - starting after Roland logo area
    int x = 130;
    tuningKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;
    cutoffKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;
    resonanceKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;
    envModKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;
    decayKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;
    accentKnob_.setBounds(x, y, knobSize, knobSize); x += knobSize + 5;

    // Master tune (smaller)
    masterTuneKnob_.setBounds(x, y + 5, smallKnobSize, smallKnobSize); x += smallKnobSize + 15;

    // Drive section
    int driveX = bounds.getWidth() - 360;
    driveTypeBox_.setBounds(driveX, y + 2, 90, 20);
    driveToneKnob_.setBounds(driveX, y + 22, smallKnobSize, smallKnobSize);
    driveDepthKnob_.setBounds(driveX + smallKnobSize + 2, y + 22, smallKnobSize, smallKnobSize);

    // Delay section
    int delayX = bounds.getWidth() - 240;
    delayTypeBox_.setBounds(delayX, y + 2, 90, 20);
    delayTimeKnob_.setBounds(delayX, y + 22, smallKnobSize, smallKnobSize);
    delayLevelKnob_.setBounds(delayX + smallKnobSize + 2, y + 22, smallKnobSize, smallKnobSize);

    // Tempo sync
    tempoSyncButton_.setBounds(bounds.getWidth() - 130, y + 30, 80, 25);

    // Volume (larger knob, far right)
    volumeKnob_.setBounds(bounds.getWidth() - 90, y - 5, 80, 80);
}
