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
    setResizable(false, false);

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
    // Dark background
    g.fillAll(TB303LookAndFeel::getBgDark());

    auto bounds = getLocalBounds().toFloat();

    // Subtle outer glow border
    g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.12f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 2.0f);

    g.setColour(TB303LookAndFeel::getBorder());
    g.drawRoundedRectangle(bounds.reduced(2.0f), 3.0f, 1.0f);
}

void TB303AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(4);

    int topHeight = 150;
    int middleHeight = 110;
    int bottomHeight = bounds.getHeight() - topHeight - middleHeight;

    topPanel_.setBounds(bounds.removeFromTop(topHeight));
    middlePanel_.setBounds(bounds.removeFromTop(middleHeight));
    bottomPanel_.setBounds(bounds);
}
