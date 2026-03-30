#include "LEDButton.h"
#include "TB303LookAndFeel.h"

LEDButton::LEDButton(const juce::String& text) : buttonText_(text) {}

void LEDButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // LED indicator (centered at top)
    float ledSize = 6.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    float ledY = 2.0f;

    // LED glow
    if (ledOn_)
    {
        g.setColour(TB303LookAndFeel::getLEDRed().withAlpha(0.25f));
        g.fillEllipse(ledX - 3.0f, ledY - 3.0f, ledSize + 6.0f, ledSize + 6.0f);
    }
    g.setColour(ledOn_ ? TB303LookAndFeel::getLEDRed() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, ledY, ledSize, ledSize);

    // Button body area
    auto buttonArea = bounds.withTrimmedTop(ledSize + 5.0f).withTrimmedBottom(14.0f).reduced(3.0f);
    if (buttonArea.getHeight() < 10.0f) return;
    buttonBounds_ = buttonArea;

    // 3D button (like hardware)
    g.setColour(juce::Colour(0xFFD4D4DC));
    g.fillRoundedRectangle(buttonArea, 2.0f);

    // Highlight (top-left)
    g.setColour(juce::Colour(0xFFEAEAF0));
    g.drawLine(buttonArea.getX() + 1, buttonArea.getY() + 1,
               buttonArea.getRight() - 1, buttonArea.getY() + 1, 1.0f);
    g.drawLine(buttonArea.getX() + 1, buttonArea.getY() + 1,
               buttonArea.getX() + 1, buttonArea.getBottom() - 1, 1.0f);

    // Shadow (bottom-right)
    g.setColour(juce::Colour(0xFF8A8A92));
    g.drawLine(buttonArea.getX() + 1, buttonArea.getBottom() - 1,
               buttonArea.getRight() - 1, buttonArea.getBottom() - 1, 1.0f);
    g.drawLine(buttonArea.getRight() - 1, buttonArea.getY() + 1,
               buttonArea.getRight() - 1, buttonArea.getBottom() - 1, 1.0f);

    // Label below button
    auto textArea = juce::Rectangle<float>(bounds.getX(), buttonArea.getBottom() + 1.0f,
                                            bounds.getWidth(), 13.0f);
    g.setColour(TB303LookAndFeel::getTextLight());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawFittedText(buttonText_, textArea.toNearestInt(), juce::Justification::centredTop, 2);
}

void LEDButton::resized() {}

void LEDButton::mouseDown(const juce::MouseEvent& /*e*/)
{
    if (toggleMode_)
        ledOn_ = !ledOn_;
    else
        ledOn_ = true;

    repaint();
    if (onClick)
        onClick(ledOn_);
}

void LEDButton::setLEDOn(bool on)
{
    if (ledOn_ != on)
    {
        ledOn_ = on;
        repaint();
    }
}
