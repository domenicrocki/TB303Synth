#include "TopPanel.h"
#include "TB303LookAndFeel.h"

TopPanel::TopPanel(juce::AudioProcessorValueTreeState& apvts)
    : apvts_(apvts),
      tuningKnob_("TUNING", "tuning", apvts),
      cutoffKnob_("CUT OFF FREQ", "cutoff", apvts),
      resonanceKnob_("RESONANCE", "resonance", apvts),
      envModKnob_("ENV MOD", "envMod", apvts),
      decayKnob_("DECAY", "decay", apvts),
      accentKnob_("ACCENT", "accent", apvts),
      masterTuneKnob_("MASTER\nTUNE", "masterTune", apvts),
      driveToneKnob_("TONE", "driveTone", apvts),
      driveDepthKnob_("DEPTH", "driveDepth", apvts),
      delayTimeKnob_("TIME", "delayTime", apvts),
      delayLevelKnob_("LEVEL", "delayLevel", apvts)
{
    waveformButton_.setButtonText("SAW");
    waveformButton_.onClick = [this]() {
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

    // VCF Trim (cosmetic, not attached to APVTS)
    vcfTrimSlider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    vcfTrimSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    vcfTrimSlider_.setRange(0.0, 1.0, 0.01);
    vcfTrimSlider_.setValue(0.5);
    addAndMakeVisible(vcfTrimSlider_);

    // Condition (cosmetic, not attached to APVTS)
    conditionSlider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    conditionSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    conditionSlider_.setRange(0.0, 1.0, 0.01);
    conditionSlider_.setValue(0.5);
    addAndMakeVisible(conditionSlider_);

    driveTypeBox_.addItemList(juce::StringArray{ "Soft", "Hard", "Tube" }, 1);
    addAndMakeVisible(driveTypeBox_);
    driveTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "driveType", driveTypeBox_);
    addAndMakeVisible(driveToneKnob_);
    addAndMakeVisible(driveDepthKnob_);

    delayTypeBox_.addItemList(juce::StringArray{ "Digital", "Tape", "PingPong" }, 1);
    addAndMakeVisible(delayTypeBox_);
    delayTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "delayType", delayTypeBox_);
    addAndMakeVisible(delayTimeKnob_);
    addAndMakeVisible(delayLevelKnob_);

    tempoSyncButton_.setButtonText("OFF  ON");
    addAndMakeVisible(tempoSyncButton_);
    tempoSyncAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "tempoSync", tempoSyncButton_);

    volumeSlider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeSlider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider_.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(volumeSlider_);
    volumeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "volume", volumeSlider_);
}

TopPanel::~TopPanel() {}

void TopPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    TB303LookAndFeel::paintSilverBg(g, bounds, false);

    // --- Far left: WAVEFORM label + wave icons + Rocki logo ---
    g.setColour(TB303LookAndFeel::textDk());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("WAVEFORM", 10, 10, 90, 12, juce::Justification::centred);

    // Wave icons (sawtooth + square simplified)
    g.setColour(TB303LookAndFeel::textDk().withAlpha(0.7f));
    // Sawtooth icon
    {
        juce::Path saw;
        saw.startNewSubPath(18.0f, 48.0f);
        saw.lineTo(38.0f, 30.0f);
        saw.lineTo(38.0f, 48.0f);
        saw.lineTo(58.0f, 30.0f);
        saw.lineTo(58.0f, 48.0f);
        g.strokePath(saw, juce::PathStrokeType(1.5f));
    }
    // Square icon
    {
        juce::Path sqr;
        sqr.startNewSubPath(62.0f, 48.0f);
        sqr.lineTo(62.0f, 30.0f);
        sqr.lineTo(74.0f, 30.0f);
        sqr.lineTo(74.0f, 48.0f);
        sqr.lineTo(86.0f, 48.0f);
        sqr.lineTo(86.0f, 30.0f);
        sqr.lineTo(98.0f, 30.0f);
        g.strokePath(sqr, juce::PathStrokeType(1.5f));
    }

    // Rocki logo
    g.setColour(TB303LookAndFeel::textDk());
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("Rocki", 10, 80, 110, 36, juce::Justification::centred);

    // --- Vertical separators ---
    auto drawSep = [&](float x) {
        g.setColour(juce::Colour(0xFF909098));
        g.drawLine(x, 10.0f, x, bounds.getHeight() - 10.0f, 1.0f);
    };

    float sep1X = 750.0f;
    float sep2X = 900.0f;
    float sep3X = 1060.0f;
    float sep4X = 1230.0f;
    float sep5X = 1370.0f;

    drawSep(sep1X);
    drawSep(sep2X);
    drawSep(sep3X);
    drawSep(sep4X);
    drawSep(sep5X);

    // --- Master Tune section labels ---
    g.setColour(TB303LookAndFeel::textDk());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("MASTER TUNE", 758, 10, 80, 10, juce::Justification::centred);
    g.drawText("VCF TRIM", 758, 60, 80, 10, juce::Justification::centred);
    g.drawText("CONDITION", 848, 60, 45, 10, juce::Justification::centred);

    // --- Drive section label ---
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("DRIVE TYPE", 908, 10, 100, 12, juce::Justification::centred);

    // --- Delay section label ---
    g.drawText("DELAY TYPE", 1068, 10, 100, 12, juce::Justification::centred);

    // --- Tempo Sync label ---
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("TEMPO SYNC", 1238, 10, 80, 10, juce::Justification::centred);

    // --- Volume label ---
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("VOLUME", 1378, 10, 110, 14, juce::Justification::centred);
}

void TopPanel::resized()
{
    int knobW = 88;
    int knobH = 88;
    int smallKnobW = 55;
    int smallKnobH = 60;
    int topY = 24;

    // Waveform button
    waveformButton_.setBounds(30, 52, 50, 22);

    // 6 main knobs
    int x = 128;
    int gap = 8;
    tuningKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    cutoffKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    resonanceKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    envModKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    decayKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    accentKnob_.setBounds(x, topY, knobW, knobH);

    // Master Tune section (between sep1 750 and sep2 900)
    masterTuneKnob_.setBounds(762, topY, smallKnobW + 15, smallKnobH);
    vcfTrimSlider_.setBounds(762, 72, 45, 45);
    conditionSlider_.setBounds(845, 72, 45, 45);

    // Drive section (between sep2 900 and sep3 1060)
    driveTypeBox_.setBounds(920, topY + 4, 120, 22);
    driveToneKnob_.setBounds(920, topY + 36, smallKnobW + 10, smallKnobH + 10);
    driveDepthKnob_.setBounds(990, topY + 36, smallKnobW + 10, smallKnobH + 10);

    // Delay section (between sep3 1060 and sep4 1230)
    delayTypeBox_.setBounds(1082, topY + 4, 120, 22);
    delayTimeKnob_.setBounds(1082, topY + 36, smallKnobW + 10, smallKnobH + 10);
    delayLevelKnob_.setBounds(1152, topY + 36, smallKnobW + 10, smallKnobH + 10);

    // Tempo Sync toggle (between sep4 1230 and sep5 1370)
    tempoSyncButton_.setBounds(1250, topY + 20, 100, 30);

    // Volume large knob (after sep5 1370)
    volumeSlider_.setBounds(1388, topY + 10, 100, 100);
}
