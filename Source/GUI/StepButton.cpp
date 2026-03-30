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
        g.setColour(TB303LookAndFeel::getLEDRed().withAlpha(0.2f));
        g.fillEllipse(ledX - 2.0f, 0.0f, ledSize + 4.0f, ledSize + 4.0f);
    }
    g.setColour(current_ ? TB303LookAndFeel::getLEDRed() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, 1.0f, ledSize, ledSize);

    // Button body
    auto buttonArea = bounds.withTrimmedTop(ledSize + 4.0f);

    // 3D toggle switch look
    juce::Colour bgCol = active_ ? juce::Colour(0xFFE0E0E8) : juce::Colour(0xFFA0A0A8);
    g.setColour(bgCol);
    g.fillRoundedRectangle(buttonArea, 2.0f);

    // Highlight/shadow for 3D effect
    if (active_)
    {
        g.setColour(juce::Colour(0xFFF0F0F8));
        g.drawLine(buttonArea.getX() + 1, buttonArea.getY() + 1,
                   buttonArea.getRight() - 1, buttonArea.getY() + 1, 1.0f);
    }
    else
    {
        g.setColour(juce::Colour(0xFF707078));
        g.drawLine(buttonArea.getX() + 1, buttonArea.getY() + 1,
                   buttonArea.getRight() - 1, buttonArea.getY() + 1, 1.0f);
    }

    g.setColour(juce::Colour(0xFF606068));
    g.drawRoundedRectangle(buttonArea, 2.0f, 0.8f);
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
    if (active_ != active) { active_ = active; repaint(); }
}

void StepButton::setCurrent(bool current)
{
    if (current_ != current) { current_ = current; repaint(); }
}
