#include "StepButton.h"
#include "TB303LookAndFeel.h"

StepButton::StepButton(int stepIndex) : stepIndex_(stepIndex) {}

void StepButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // LED at top
    float ledSize = 5.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    if (current_)
    {
        g.setColour(TB303LookAndFeel::getLEDOn().withAlpha(0.2f));
        g.fillEllipse(ledX - 3.0f, -1.0f, ledSize + 6.0f, ledSize + 6.0f);
    }
    g.setColour(current_ ? TB303LookAndFeel::getLEDOn() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, 1.0f, ledSize, ledSize);

    // Button body
    auto buttonArea = bounds.withTrimmedTop(ledSize + 4.0f);

    juce::Colour bgCol = active_ ? TB303LookAndFeel::getBgLight().brighter(0.2f) : TB303LookAndFeel::getBgLight();
    g.setColour(bgCol);
    g.fillRoundedRectangle(buttonArea, 2.0f);

    juce::Colour borderCol = active_ ? TB303LookAndFeel::getNeonCyan().withAlpha(0.5f) : TB303LookAndFeel::getBorderLight();
    g.setColour(borderCol);
    g.drawRoundedRectangle(buttonArea, 2.0f, 0.8f);

    if (active_)
    {
        g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.08f));
        g.fillRoundedRectangle(buttonArea, 2.0f);
    }
}

void StepButton::mouseDown(const juce::MouseEvent& /*e*/)
{
    active_ = !active_;
    repaint();
    if (onToggle) onToggle(stepIndex_, active_);
}

void StepButton::setActive(bool active)
{
    if (active_ != active) { active_ = active; repaint(); }
}

void StepButton::setCurrent(bool current)
{
    if (current_ != current) { current_ = current; repaint(); }
}
