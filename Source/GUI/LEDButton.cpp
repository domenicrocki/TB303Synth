#include "LEDButton.h"
#include "TB303LookAndFeel.h"

LEDButton::LEDButton(const juce::String& text) : buttonText_(text) {}

void LEDButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // LED indicator
    float ledSize = 6.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    float ledY = 2.0f;

    g.setColour(ledOn_ ? TB303LookAndFeel::getLEDRed() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, ledY, ledSize, ledSize);
    if (ledOn_)
    {
        // Glow effect
        g.setColour(TB303LookAndFeel::getLEDRed().withAlpha(0.3f));
        g.fillEllipse(ledX - 2.0f, ledY - 2.0f, ledSize + 4.0f, ledSize + 4.0f);
    }

    // Button body
    auto buttonArea = bounds.withTrimmedTop(ledSize + 4.0f).reduced(2.0f);
    buttonBounds_ = buttonArea;

    juce::Colour bgCol = juce::Colour(0xFFD0D0D8);
    g.setColour(bgCol);
    g.fillRoundedRectangle(buttonArea, 3.0f);

    // 3D border
    g.setColour(juce::Colour(0xFFE8E8F0));
    g.drawLine(buttonArea.getX(), buttonArea.getY(), buttonArea.getRight(), buttonArea.getY(), 1.0f);
    g.drawLine(buttonArea.getX(), buttonArea.getY(), buttonArea.getX(), buttonArea.getBottom(), 1.0f);
    g.setColour(juce::Colour(0xFF808088));
    g.drawLine(buttonArea.getX(), buttonArea.getBottom(), buttonArea.getRight(), buttonArea.getBottom(), 1.0f);
    g.drawLine(buttonArea.getRight(), buttonArea.getY(), buttonArea.getRight(), buttonArea.getBottom(), 1.0f);

    // Text
    g.setColour(TB303LookAndFeel::getTextColor());
    g.setFont(juce::Font(9.0f, juce::Font::bold));

    // Label below button
    auto textArea = juce::Rectangle<float>(bounds.getX(), buttonArea.getBottom() + 1.0f,
                                            bounds.getWidth(), bounds.getBottom() - buttonArea.getBottom() - 1.0f);
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
