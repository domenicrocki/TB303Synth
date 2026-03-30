#include "StepButton.h"
#include "TB303LookAndFeel.h"

StepButton::StepButton(int stepIndex) : stepIndex_(stepIndex) {}

void StepButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    // LED
    float ledSize = 5.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    g.setColour(current_ ? TB303LookAndFeel::getLEDRed() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, 1.0f, ledSize, ledSize);

    // Button
    auto buttonArea = bounds.withTrimmedTop(ledSize + 3.0f);

    juce::Colour bgCol = active_ ? juce::Colour(0xFFE0E0E8) : juce::Colour(0xFFA0A0A8);
    g.setColour(bgCol);
    g.fillRoundedRectangle(buttonArea, 2.0f);

    g.setColour(juce::Colour(0xFF606068));
    g.drawRoundedRectangle(buttonArea, 2.0f, 1.0f);

    // Step number
    g.setColour(TB303LookAndFeel::getTextColor());
    g.setFont(juce::Font(9.0f));
    g.drawText(juce::String(stepIndex_ + 1), buttonArea, juce::Justification::centred);
}

void StepButton::mouseDown(const juce::MouseEvent& /*e*/)
{
    active_ = !active_;
    repaint();
    if (onToggle)
        onToggle(stepIndex_, active_);
}

void StepButton::setActive(bool active)
{
    if (active_ != active)
    {
        active_ = active;
        repaint();
    }
}

void StepButton::setCurrent(bool current)
{
    if (current_ != current)
    {
        current_ = current;
        repaint();
    }
}
