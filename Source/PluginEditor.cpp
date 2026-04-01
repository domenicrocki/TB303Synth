#include "PluginEditor.h"

TB303AudioProcessorEditor::TB303AudioProcessorEditor(TB303AudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor_(p),
      topPanel_(p),
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
    g.fillAll(TB303Colors::background());
}

void TB303AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    int topHeight    = 240;
    int middleHeight = 0;
    int bottomHeight = bounds.getHeight() - topHeight - middleHeight;

    topPanel_.setBounds(bounds.removeFromTop(topHeight));
    middlePanel_.setBounds(bounds.removeFromTop(middleHeight));
    bottomPanel_.setBounds(bounds.withHeight(bottomHeight));
}
