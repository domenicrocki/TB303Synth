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

    tempoSyncButton_.setButtonText("SYNC");
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
    TB303LookAndFeel::drawFuturisticPanel(g, bounds, TB303LookAndFeel::getNeonCyan());

    // WAVEFORM label
    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("WAVEFORM", 8, 8, 80, 10, juce::Justification::centredLeft);

    // Rocki logo
    g.setColour(TB303LookAndFeel::getNeonCyan());
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Rocki", 12, 55, 110, 32, juce::Justification::centredLeft);
    // Glow effect for logo
    g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.08f));
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Rocki", 11, 54, 110, 32, juce::Justification::centredLeft);

    // Section dividers (glowing lines)
    float divY1 = 12.0f;
    float divY2 = bounds.getHeight() - 12.0f;

    float driveDiv = bounds.getWidth() - 380.0f;
    TB303LookAndFeel::drawGlowLine(g, driveDiv, divY1, driveDiv, divY2, TB303LookAndFeel::getNeonBlue());

    float delayDiv = bounds.getWidth() - 245.0f;
    TB303LookAndFeel::drawGlowLine(g, delayDiv, divY1, delayDiv, divY2, TB303LookAndFeel::getNeonBlue());

    float volDiv = bounds.getWidth() - 110.0f;
    TB303LookAndFeel::drawGlowLine(g, volDiv, divY1, volDiv, divY2, TB303LookAndFeel::getNeonBlue());

    // Section labels
    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));

    int drvX = static_cast<int>(driveDiv) + 8;
    g.drawText("DRIVE TYPE", drvX, 8, 80, 10, juce::Justification::centredLeft);
    int dlX = static_cast<int>(delayDiv) + 8;
    g.drawText("DELAY TYPE", dlX, 8, 80, 10, juce::Justification::centredLeft);

    g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.7f));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("VOLUME", static_cast<int>(volDiv) + 8, 8, 80, 12, juce::Justification::centredLeft);
}

void TopPanel::resized()
{
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    int knobW = 78;
    int knobH = 78;
    int smallKnobW = 58;
    int smallKnobH = 64;
    int topY = 22;

    waveformButton_.setBounds(25, 28, 55, 22);

    int x = 125;
    int gap = 5;
    tuningKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    cutoffKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    resonanceKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    envModKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    decayKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;
    accentKnob_.setBounds(x, topY, knobW, knobH); x += knobW + gap;

    masterTuneKnob_.setBounds(x + 8, topY + 10, smallKnobW, smallKnobH);

    int driveX = w - 372;
    driveTypeBox_.setBounds(driveX, topY + 2, 90, 20);
    driveToneKnob_.setBounds(driveX, topY + 26, smallKnobW, smallKnobH);
    driveDepthKnob_.setBounds(driveX + smallKnobW + 4, topY + 26, smallKnobW, smallKnobH);

    int dlX = w - 237;
    delayTypeBox_.setBounds(dlX, topY + 2, 80, 20);
    delayTimeKnob_.setBounds(dlX, topY + 26, smallKnobW, smallKnobH);
    delayLevelKnob_.setBounds(dlX + smallKnobW + 4, topY + 26, smallKnobW, smallKnobH);

    tempoSyncButton_.setBounds(dlX + 85, topY + 28, 45, 25);

    volumeSlider_.setBounds(w - 100, topY - 5, 90, 90);
}
