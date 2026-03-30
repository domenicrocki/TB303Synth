#include "StepButton.h"
#include "TB303LookAndFeel.h"

StepButton::StepButton(int stepIndex) : stepIndex_(stepIndex) {}

void StepButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // LED at top
    float ledSize = 5.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    TB303LookAndFeel::paintLED(g, ledX, 1.0f, ledSize, current_);

    // Toggle switch body (silver metallic look)
    auto switchArea = bounds.withTrimmedTop(ledSize + 4.0f);

    // Switch groove background
    g.setColour(juce::Colour(0xFF404048));
    g.fillRoundedRectangle(switchArea, 3.0f);

    // Toggle lever - top or bottom depending on active state
    auto leverH = switchArea.getHeight() * 0.48f;
    juce::Rectangle<float> leverArea;
    if (active_)
        leverArea = switchArea.withHeight(leverH);
    else
        leverArea = switchArea.withTrimmedTop(switchArea.getHeight() - leverH);

    // Silver metallic lever
    {
        juce::ColourGradient gr(juce::Colour(0xFFE0E0E4), leverArea.getX(), leverArea.getY(),
                                 juce::Colour(0xFFA0A0A8), leverArea.getX(), leverArea.getBottom(), false);
        g.setGradientFill(gr);
        g.fillRoundedRectangle(leverArea.reduced(1.0f), 2.0f);
    }

    // Lever highlight
    g.setColour(juce::Colour(0xFFEEEEF0));
    g.drawLine(leverArea.getX() + 3, leverArea.getY() + 2,
               leverArea.getRight() - 3, leverArea.getY() + 2, 0.8f);

    // Border
    g.setColour(juce::Colour(0xFF606068));
    g.drawRoundedRectangle(switchArea, 3.0f, 1.0f);
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
