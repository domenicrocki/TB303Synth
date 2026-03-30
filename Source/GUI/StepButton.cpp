#include "StepButton.h"
#include "TB303LookAndFeel.h"

StepButton::StepButton(int stepIndex) : stepIndex_(stepIndex) {}

void StepButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // LED at top
    float ledSize = 6.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    TB303LookAndFeel::paintLED(g, ledX, 1.0f, ledSize, current_);

    // Toggle button body
    auto btnArea = bounds.withTrimmedTop(ledSize + 4.0f);

    // Background
    auto baseColor = active_ ? juce::Colour(0xFF2A2A3E) : juce::Colour(0xFF1A1A28);
    g.setColour(baseColor);
    g.fillRoundedRectangle(btnArea, 3.0f);

    // Border - accent when active
    g.setColour(active_ ? TB303Colors::pink() : TB303Colors::panelBorder());
    g.drawRoundedRectangle(btnArea, 3.0f, 1.0f);

    // Active indicator dot
    if (active_)
    {
        g.setColour(TB303Colors::pink());
        float dotSz = 4.0f;
        g.fillEllipse(btnArea.getCentreX() - dotSz * 0.5f,
                      btnArea.getCentreY() - dotSz * 0.5f, dotSz, dotSz);
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
