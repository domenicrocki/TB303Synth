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
    g.fillAll(juce::Colour(0xFFC8C8CC));

    auto bounds = getLocalBounds().toFloat();

    // Outer border
    g.setColour(juce::Colour(0xFF909098));
    g.drawRect(bounds, 2.0f);

    // Inner bevel highlight
    g.setColour(juce::Colour(0xFFE0E0E4));
    g.drawLine(2.0f, 2.0f, bounds.getWidth() - 2.0f, 2.0f, 1.0f);
    g.drawLine(2.0f, 2.0f, 2.0f, bounds.getHeight() - 2.0f, 1.0f);
}

void TB303AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(3);

    // Top panel: knobs area ~35% of height
    int topHeight = 150;
    // Middle panel: controls ~25%
    int middleHeight = 115;
    // Bottom panel: keyboard/sequencer ~40%
    int bottomHeight = bounds.getHeight() - topHeight - middleHeight;

    topPanel_.setBounds(bounds.removeFromTop(topHeight));
    middlePanel_.setBounds(bounds.removeFromTop(middleHeight));
    bottomPanel_.setBounds(bounds);
}
