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

    TB303LookAndFeel::paintLED(g, ledX, ledY, ledSize, ledOn_);

    // Button body (3D beveled silver button)
    auto buttonArea = bounds.withTrimmedTop(ledSize + 5.0f).withTrimmedBottom(14.0f).reduced(3.0f);
    if (buttonArea.getHeight() < 8.0f) return;
    buttonBounds_ = buttonArea;

    TB303LookAndFeel::paint3DButton(g, buttonArea, ledOn_);

    // Label below
    auto textArea = juce::Rectangle<float>(bounds.getX(), buttonArea.getBottom() + 1.0f,
                                            bounds.getWidth(), 13.0f);
    g.setColour(TB303LookAndFeel::textDk());
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
