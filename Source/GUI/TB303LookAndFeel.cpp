#include "TB303LookAndFeel.h"

TB303LookAndFeel::TB303LookAndFeel()
{
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, getTextColor());
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFFE8E8EC));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF808080));
    setColour(juce::ComboBox::textColourId, getTextColor());
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFFE8E8EC));
    setColour(juce::PopupMenu::textColourId, getTextColor());
}

void TB303LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Knob body - metallic gradient
    {
        juce::ColourGradient gradient(juce::Colour(0xFFE8E8EC), centreX - radius, centreY - radius,
                                       juce::Colour(0xFF909098), centreX + radius, centreY + radius, true);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);
    }

    // Outer ring shadow
    g.setColour(juce::Colour(0xFF505058));
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // Inner circle (darker)
    auto innerRadius = radius * 0.75f;
    {
        juce::ColourGradient gradient(juce::Colour(0xFFD0D0D4), centreX - innerRadius * 0.5f, centreY - innerRadius * 0.5f,
                                       juce::Colour(0xFF808088), centreX + innerRadius * 0.5f, centreY + innerRadius * 0.5f, true);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);
    }

    // Indicator line
    juce::Path indicator;
    auto indicatorLength = radius * 0.85f;
    auto indicatorThickness = 2.5f;
    indicator.addRectangle(-indicatorThickness * 0.5f, -indicatorLength, indicatorThickness, indicatorLength * 0.45f);

    g.setColour(juce::Colours::white);
    g.fillPath(indicator, juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // Small dot at center
    g.setColour(juce::Colour(0xFF404048));
    g.fillEllipse(centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}

void TB303LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& /*backgroundColour*/,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    // Silver button style
    juce::Colour baseColour = shouldDrawButtonAsDown ? juce::Colour(0xFFA0A0A4)
                             : shouldDrawButtonAsHighlighted ? juce::Colour(0xFFD8D8DC)
                             : juce::Colour(0xFFC8C8CC);

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(juce::Colour(0xFF606068));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
}

void TB303LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);

    // Button body
    juce::Colour bgCol = button.getToggleState() ? juce::Colour(0xFFB0B0B8) : juce::Colour(0xFFD0D0D4);
    if (shouldDrawButtonAsHighlighted)
        bgCol = bgCol.brighter(0.1f);

    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(juce::Colour(0xFF606068));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

    // Text
    g.setColour(getTextColor());
    g.setFont(juce::Font(11.0f));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2), juce::Justification::centred, 1);
}

void TB303LookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                     bool /*isButtonDown*/, int /*buttonX*/, int /*buttonY*/,
                                     int /*buttonW*/, int /*buttonH*/, juce::ComboBox& /*box*/)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    g.setColour(juce::Colour(0xFFE8E8EC));
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(juce::Colour(0xFF808080));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);

    // Arrow
    auto arrowZone = juce::Rectangle<float>(static_cast<float>(width) - 20.0f, 0.0f, 18.0f, static_cast<float>(height));
    juce::Path arrow;
    arrow.addTriangle(arrowZone.getCentreX() - 4.0f, arrowZone.getCentreY() - 2.0f,
                      arrowZone.getCentreX() + 4.0f, arrowZone.getCentreY() - 2.0f,
                      arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
    g.setColour(juce::Colour(0xFF404048));
    g.fillPath(arrow);
}
