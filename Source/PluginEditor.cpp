#include "PluginEditor.h"

TB303AudioProcessorEditor::TB303AudioProcessorEditor(TB303AudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor_(p),
      topPanel_(p.getAPVTS()),
      middlePanel_(p),
      bottomPanel_(p)
{
    setLookAndFeel(&lookAndFeel_);
    setSize(EDITOR_WIDTH, EDITOR_HEIGHT);

    addAndMakeVisible(topPanel_);
    addAndMakeVisible(middlePanel_);
    addAndMakeVisible(bottomPanel_);
}

TB303AudioProcessorEditor::~TB303AudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void TB303AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Overall background
    g.fillAll(TB303LookAndFeel::getSilverBackground());

    // Bottom screw-like details
    g.setColour(juce::Colour(0xFF808088));
    float screwSize = 8.0f;
    float margin = 10.0f;
    auto bounds = getLocalBounds().toFloat();

    // Corner screws
    g.fillEllipse(margin, margin, screwSize, screwSize);
    g.fillEllipse(bounds.getWidth() - margin - screwSize, margin, screwSize, screwSize);
    g.fillEllipse(margin, bounds.getHeight() - margin - screwSize, screwSize, screwSize);
    g.fillEllipse(bounds.getWidth() - margin - screwSize, bounds.getHeight() - margin - screwSize, screwSize, screwSize);
}

void TB303AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    int topHeight = 95;
    int middleHeight = 90;
    int bottomHeight = bounds.getHeight() - topHeight - middleHeight;

    topPanel_.setBounds(bounds.removeFromTop(topHeight));
    middlePanel_.setBounds(bounds.removeFromTop(middleHeight));
    bottomPanel_.setBounds(bounds);
}
