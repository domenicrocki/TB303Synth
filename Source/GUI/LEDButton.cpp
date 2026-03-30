#include "LEDButton.h"
#include "TB303LookAndFeel.h"

LEDButton::LEDButton(const juce::String& text) : buttonText_(text) {}

void LEDButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // LED at top center
    float ledSize = 8.0f;
    float ledX = bounds.getCentreX() - ledSize * 0.5f;
    float ledY = 4.0f;

    TB303LookAndFeel::paintLED(g, ledX, ledY, ledSize, ledOn_);

    // Button body
    auto buttonArea = bounds.withTrimmedTop(ledSize + 8.0f).withTrimmedBottom(16.0f).reduced(4.0f);
    if (buttonArea.getHeight() < 8.0f)
        buttonArea = bounds.withTrimmedTop(ledSize + 6.0f).reduced(4.0f);
    buttonBounds_ = buttonArea;

    auto baseColor = juce::Colour(0xFF1E1E2E);
    if (ledOn_) baseColor = baseColor.brighter(0.1f);
    g.setColour(baseColor);
    g.fillRoundedRectangle(buttonArea, 4.0f);

    g.setColour(ledOn_ ? accentColor_.withAlpha(0.6f) : TB303Colors::panelBorder());
    g.drawRoundedRectangle(buttonArea, 4.0f, 1.0f);

    // Label below button
    auto textArea = juce::Rectangle<float>(bounds.getX(), buttonArea.getBottom() + 2.0f,
                                           bounds.getWidth(), 14.0f);
    g.setColour(TB303Colors::textDim());
    g.setFont(juce::Font(8.5f, juce::Font::bold));
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
    if (onClick) onClick(ledOn_);
}

void LEDButton::mouseUp(const juce::MouseEvent& /*e*/)
{
    // Momentary mode: release LED when mouse released
    if (!toggleMode_ && ledOn_)
    {
        ledOn_ = false;
        repaint();
    }
}

void LEDButton::setLEDOn(bool on)
{
    if (ledOn_ != on) { ledOn_ = on; repaint(); }
}
