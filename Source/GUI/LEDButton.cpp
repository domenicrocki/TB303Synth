#include "LEDButton.h"
#include "TB303LookAndFeel.h"

LEDButton::LEDButton(const juce::String& text) : buttonText_(text) {}

void LEDButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // LED at top center
    float ledSize = 6.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    float ledY = 2.0f;

    if (ledOn_)
    {
        // Neon glow
        g.setColour(TB303LookAndFeel::getLEDOn().withAlpha(0.2f));
        g.fillEllipse(ledX - 4.0f, ledY - 4.0f, ledSize + 8.0f, ledSize + 8.0f);
    }
    g.setColour(ledOn_ ? TB303LookAndFeel::getLEDOn() : TB303LookAndFeel::getLEDOff());
    g.fillEllipse(ledX, ledY, ledSize, ledSize);

    // Button body
    auto buttonArea = bounds.withTrimmedTop(ledSize + 5.0f).withTrimmedBottom(14.0f).reduced(3.0f);
    if (buttonArea.getHeight() < 8.0f) return;
    buttonBounds_ = buttonArea;

    g.setColour(TB303LookAndFeel::getBgLight());
    g.fillRoundedRectangle(buttonArea, 3.0f);

    juce::Colour borderCol = ledOn_ ? TB303LookAndFeel::getNeonCyan().withAlpha(0.5f) : TB303LookAndFeel::getBorderLight();
    g.setColour(borderCol);
    g.drawRoundedRectangle(buttonArea, 3.0f, 1.0f);

    if (ledOn_)
    {
        g.setColour(TB303LookAndFeel::getNeonCyan().withAlpha(0.06f));
        g.fillRoundedRectangle(buttonArea, 3.0f);
    }

    // Label
    auto textArea = juce::Rectangle<float>(bounds.getX(), buttonArea.getBottom() + 1.0f,
                                            bounds.getWidth(), 13.0f);
    g.setColour(TB303LookAndFeel::getTextDim());
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawFittedText(buttonText_, textArea.toNearestInt(), juce::Justification::centredTop, 2);
}

void LEDButton::resized() {}

void LEDButton::mouseDown(const juce::MouseEvent& /*e*/)
{
    if (toggleMode_) ledOn_ = !ledOn_;
    else ledOn_ = true;
    repaint();
    if (onClick) onClick(ledOn_);
}

void LEDButton::setLEDOn(bool on)
{
    if (ledOn_ != on) { ledOn_ = on; repaint(); }
}
