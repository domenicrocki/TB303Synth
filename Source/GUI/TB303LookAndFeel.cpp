#include "TB303LookAndFeel.h"

TB303LookAndFeel::TB303LookAndFeel()
{
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, getTextDark());
    setColour(juce::ComboBox::backgroundColourId, getDisplayBg());
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF808080));
    setColour(juce::ComboBox::textColourId, getTextDark());
    setColour(juce::PopupMenu::backgroundColourId, getDisplayBg());
    setColour(juce::PopupMenu::textColourId, getTextDark());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF4060A0));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void TB303LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Outer shadow
    g.setColour(juce::Colour(0x40000000));
    g.fillEllipse(centreX - radius + 1.0f, centreY - radius + 1.5f, radius * 2.0f, radius * 2.0f);

    // Knob body - metallic gradient
    {
        juce::ColourGradient gradient(
            juce::Colour(0xFFE8E8F0), centreX - radius * 0.7f, centreY - radius * 0.7f,
            juce::Colour(0xFF8A8A92), centreX + radius * 0.7f, centreY + radius * 0.7f, true);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);
    }

    // Outer ring
    g.setColour(juce::Colour(0xFF606068));
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 1.2f);

    // Inner circle
    auto innerR = radius * 0.7f;
    {
        juce::ColourGradient gradient(
            juce::Colour(0xFFD8D8E0), centreX - innerR * 0.5f, centreY - innerR * 0.5f,
            juce::Colour(0xFF909098), centreX + innerR * 0.5f, centreY + innerR * 0.5f, true);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - innerR, centreY - innerR, innerR * 2.0f, innerR * 2.0f);
    }

    // Indicator line (white)
    juce::Path indicator;
    auto indicatorLen = radius * 0.88f;
    auto indicatorW = juce::jmax(1.5f, radius * 0.08f);
    indicator.addRoundedRectangle(-indicatorW * 0.5f, -indicatorLen, indicatorW, indicatorLen * 0.4f, 1.0f);

    g.setColour(juce::Colours::white);
    g.fillPath(indicator, juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // Center dot
    g.setColour(juce::Colour(0xFF505058));
    g.fillEllipse(centreX - 1.5f, centreY - 1.5f, 3.0f, 3.0f);
}

void TB303LookAndFeel::drawLargeKnob(juce::Graphics& g, juce::Rectangle<float> bounds,
                                       float sliderPos, float startAngle, float endAngle)
{
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = startAngle + sliderPos * (endAngle - startAngle);

    // Outer notched ring
    g.setColour(juce::Colour(0xFF606068));
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 2.0f);

    // Tick marks around the ring
    int numTicks = 31;
    for (int i = 0; i < numTicks; ++i)
    {
        float tickAngle = startAngle + (static_cast<float>(i) / static_cast<float>(numTicks - 1)) * (endAngle - startAngle);
        float innerTick = radius - 4.0f;
        float outerTick = radius;
        float x1 = centreX + innerTick * std::sin(tickAngle);
        float y1 = centreY - innerTick * std::cos(tickAngle);
        float x2 = centreX + outerTick * std::sin(tickAngle);
        float y2 = centreY - outerTick * std::cos(tickAngle);
        g.setColour(juce::Colour(0xFF505058));
        g.drawLine(x1, y1, x2, y2, 0.8f);
    }

    // Knob body
    auto knobR = radius * 0.82f;
    {
        juce::ColourGradient gradient(
            juce::Colour(0xFFE0E0E8), centreX - knobR * 0.6f, centreY - knobR * 0.6f,
            juce::Colour(0xFF8A8A92), centreX + knobR * 0.6f, centreY + knobR * 0.6f, true);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - knobR, centreY - knobR, knobR * 2.0f, knobR * 2.0f);
    }

    // Indicator
    juce::Path indicator;
    auto indicatorLen = knobR * 0.9f;
    indicator.addRoundedRectangle(-1.5f, -indicatorLen, 3.0f, indicatorLen * 0.35f, 1.0f);
    g.setColour(juce::Colours::white);
    g.fillPath(indicator, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
}

void TB303LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& /*backgroundColour*/,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

    juce::Colour baseColour = shouldDrawButtonAsDown ? juce::Colour(0xFFB0B0B4)
                             : shouldDrawButtonAsHighlighted ? juce::Colour(0xFFDCDCE0)
                             : juce::Colour(0xFFD0D0D4);

    // 3D button effect
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 2.0f);

    // Top-left highlight
    g.setColour(shouldDrawButtonAsDown ? juce::Colour(0xFF909098) : juce::Colour(0xFFE8E8EC));
    g.drawLine(bounds.getX() + 1, bounds.getY() + 1, bounds.getRight() - 1, bounds.getY() + 1, 1.0f);
    g.drawLine(bounds.getX() + 1, bounds.getY() + 1, bounds.getX() + 1, bounds.getBottom() - 1, 1.0f);

    // Bottom-right shadow
    g.setColour(shouldDrawButtonAsDown ? juce::Colour(0xFFE0E0E4) : juce::Colour(0xFF808088));
    g.drawLine(bounds.getX() + 1, bounds.getBottom() - 1, bounds.getRight() - 1, bounds.getBottom() - 1, 1.0f);
    g.drawLine(bounds.getRight() - 1, bounds.getY() + 1, bounds.getRight() - 1, bounds.getBottom() - 1, 1.0f);
}

void TB303LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    juce::Colour bgCol = button.getToggleState() ? juce::Colour(0xFFB8B8C0) : juce::Colour(0xFFD0D0D8);
    if (shouldDrawButtonAsHighlighted)
        bgCol = bgCol.brighter(0.05f);

    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 2.0f);

    g.setColour(juce::Colour(0xFF707078));
    g.drawRoundedRectangle(bounds, 2.0f, 1.0f);

    g.setColour(getTextDark());
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 2);
}

void TB303LookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                     bool /*isButtonDown*/, int /*buttonX*/, int /*buttonY*/,
                                     int /*buttonW*/, int /*buttonH*/, juce::ComboBox& /*box*/)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    g.setColour(getDisplayBg());
    g.fillRoundedRectangle(bounds, 2.0f);
    g.setColour(juce::Colour(0xFF707078));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 2.0f, 1.0f);

    // Arrows
    float arrowX = static_cast<float>(width) - 16.0f;
    float midY = static_cast<float>(height) * 0.5f;
    juce::Path upArrow, downArrow;
    upArrow.addTriangle(arrowX, midY - 2.0f, arrowX + 8.0f, midY - 2.0f, arrowX + 4.0f, midY - 6.0f);
    downArrow.addTriangle(arrowX, midY + 2.0f, arrowX + 8.0f, midY + 2.0f, arrowX + 4.0f, midY + 6.0f);
    g.setColour(juce::Colour(0xFF404048));
    g.fillPath(upArrow);
    g.fillPath(downArrow);
}

void TB303LookAndFeel::drawSilverPanel(juce::Graphics& g, juce::Rectangle<float> bounds, bool darker)
{
    juce::Colour top = darker ? juce::Colour(0xFFCCCCD0) : juce::Colour(0xFFD8D8DC);
    juce::Colour bottom = darker ? juce::Colour(0xFFB8B8BC) : juce::Colour(0xFFC4C4C8);

    juce::ColourGradient gradient(top, 0.0f, bounds.getY(), bottom, 0.0f, bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // Top highlight
    g.setColour(juce::Colour(0xFFE8E8EC));
    g.drawLine(bounds.getX(), bounds.getY(), bounds.getRight(), bounds.getY(), 1.0f);
    // Bottom shadow
    g.setColour(juce::Colour(0xFFA0A0A8));
    g.drawLine(bounds.getX(), bounds.getBottom() - 1.0f, bounds.getRight(), bounds.getBottom() - 1.0f, 1.0f);
}

void TB303LookAndFeel::drawDarkPanel(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(getDarkPanel());
    g.fillRect(bounds);

    // Inner lighter area
    auto inner = bounds.reduced(6.0f);
    g.setColour(getDarkPanelLight());
    g.fillRoundedRectangle(inner, 4.0f);

    // Subtle inner border
    g.setColour(juce::Colour(0xFF1A1A20));
    g.drawRoundedRectangle(inner, 4.0f, 1.0f);
}
