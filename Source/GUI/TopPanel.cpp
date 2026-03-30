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
    // Waveform button
    waveformButton_.setButtonText("SAW");
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
    delayTypeBox_.addItemList(juce::StringArray{ "Digital", "Tape", "PingPong" }, 1);
    addAndMakeVisible(delayTypeBox_);
    delayTypeAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "delayType", delayTypeBox_);
    addAndMakeVisible(delayTimeKnob_);
    addAndMakeVisible(delayLevelKnob_);

    // Tempo sync
    tempoSyncButton_.setButtonText("ON");
    addAndMakeVisible(tempoSyncButton_);
    tempoSyncAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "tempoSync", tempoSyncButton_);

    // Volume - large custom knob
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
    TB303LookAndFeel::drawSilverPanel(g, bounds);

    g.setColour(TB303LookAndFeel::getTextDark());

    // WAVEFORM label
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("WAVEFORM", 8, 4, 80, 10, juce::Justification::centredLeft);

    // Waveform icons (saw, triangle, square)
    g.setFont(juce::Font(10.0f));
    g.drawText("M", 12, 14, 12, 12, juce::Justification::centred);  // saw-like
    g.drawText("A", 28, 14, 12, 12, juce::Justification::centred);  // triangle-like
    g.drawText("n", 44, 14, 14, 12, juce::Justification::centred);  // square-like

    // Roland "R" logo symbol
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    juce::Path rLogo;
    auto rBounds = juce::Rectangle<float>(14.0f, 52.0f, 22.0f, 22.0f);
    g.setColour(TB303LookAndFeel::getTextDark());
    g.drawRoundedRectangle(rBounds, 3.0f, 1.5f);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("R", rBounds.translated(1.0f, 0.0f), juce::Justification::centred);

    // Roland text
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("Roland", 38, 50, 90, 26, juce::Justification::centredLeft);

    // Separator lines between sections
    g.setColour(juce::Colour(0xFFA0A0A8));
    float rightSectionStart = bounds.getWidth() - 380.0f;
    g.drawLine(rightSectionStart, 8.0f, rightSectionStart, bounds.getHeight() - 8.0f, 1.0f);

    float delayStart = bounds.getWidth() - 245.0f;
    g.drawLine(delayStart, 8.0f, delayStart, bounds.getHeight() - 8.0f, 1.0f);

    float volumeStart = bounds.getWidth() - 110.0f;
    g.drawLine(volumeStart, 8.0f, volumeStart, bounds.getHeight() - 8.0f, 1.0f);

    // Section labels
    g.setColour(TB303LookAndFeel::getTextDark());
    g.setFont(juce::Font(8.0f, juce::Font::bold));

    int driveX = static_cast<int>(rightSectionStart) + 5;
    g.drawText("DRIVE TYPE", driveX, 4, 80, 10, juce::Justification::centredLeft);

    int dlX = static_cast<int>(delayStart) + 5;
    g.drawText("DELAY TYPE", dlX, 4, 80, 10, juce::Justification::centredLeft);

    g.drawText("TEMPO SYNC", dlX + 85, 4, 70, 10, juce::Justification::centredLeft);

    // Volume label
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("VOLUME", static_cast<int>(volumeStart) + 5, 4, 80, 12, juce::Justification::centredLeft);

    // PANEL label at top right
    g.setFont(juce::Font(7.0f, juce::Font::bold));
    g.drawText("PANEL", static_cast<int>(bounds.getWidth()) - 45, 2, 40, 10, juce::Justification::centred);
}

void TopPanel::resized()
{
    auto bounds = getLocalBounds();
    int w = bounds.getWidth();
    int knobW = 80;
    int knobH = 80;
    int smallKnobW = 60;
    int smallKnobH = 65;
    int topY = 22;

    // Waveform button
    waveformButton_.setBounds(25, 30, 55, 20);

    // Main synth knobs - horizontal row starting after Roland logo area
    int x = 120;
    int knobGap = 6;
    tuningKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;
    cutoffKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;
    resonanceKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;
    envModKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;
    decayKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;
    accentKnob_.setBounds(x, topY, knobW, knobH); x += knobW + knobGap;

    // Master tune (smaller, after accent)
    masterTuneKnob_.setBounds(x + 10, topY + 10, smallKnobW, smallKnobH);

    // Drive section
    int driveX = w - 375;
    driveTypeBox_.setBounds(driveX, topY + 2, 90, 20);
    driveToneKnob_.setBounds(driveX, topY + 26, smallKnobW, smallKnobH);
    driveDepthKnob_.setBounds(driveX + smallKnobW + 4, topY + 26, smallKnobW, smallKnobH);

    // Delay section
    int dlX = w - 240;
    delayTypeBox_.setBounds(dlX, topY + 2, 80, 20);
    delayTimeKnob_.setBounds(dlX, topY + 26, smallKnobW, smallKnobH);
    delayLevelKnob_.setBounds(dlX + smallKnobW + 4, topY + 26, smallKnobW, smallKnobH);

    // Tempo sync
    tempoSyncButton_.setBounds(dlX + 85, topY + 26, 45, 25);

    // Volume (large knob, far right)
    int volX = w - 100;
    volumeSlider_.setBounds(volX, topY - 5, 90, 90);
}
