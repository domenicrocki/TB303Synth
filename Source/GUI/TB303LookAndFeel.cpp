#include "TB303LookAndFeel.h"

TB303LookAndFeel::TB303LookAndFeel()
{
    // Dark futuristic theme
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, getTextBright());
    setColour(juce::ComboBox::backgroundColourId, getBgLight());
    setColour(juce::ComboBox::outlineColourId, getBorder());
    setColour(juce::ComboBox::textColourId, getTextBright());
    setColour(juce::ComboBox::arrowColourId, getNeonCyan());
    setColour(juce::PopupMenu::backgroundColourId, getBgMid());
    setColour(juce::PopupMenu::textColourId, getTextBright());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, getNeonBlue().withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId, getNeonCyan());
}

void TB303LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(3.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx = bounds.getCentreX();
    auto cy = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Outer glow ring (track)
    float trackRadius = radius;
    {
        juce::Path track;
        track.addCentredArc(cx, cy, trackRadius, trackRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(getKnobRing());
        g.strokePath(track, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Active arc (neon cyan)
    {
        juce::Path activeArc;
        activeArc.addCentredArc(cx, cy, trackRadius, trackRadius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(getNeonCyan());
        g.strokePath(activeArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Glow effect
        g.setColour(getNeonCyan().withAlpha(0.15f));
        g.strokePath(activeArc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Knob body
    auto knobR = radius * 0.7f;
    {
        juce::ColourGradient gradient(
            juce::Colour(0xFF2A2A3A), cx, cy - knobR,
            juce::Colour(0xFF151520), cx, cy + knobR, false);
        g.setGradientFill(gradient);
        g.fillEllipse(cx - knobR, cy - knobR, knobR * 2.0f, knobR * 2.0f);
    }

    // Knob border
    g.setColour(juce::Colour(0xFF3A3A50));
    g.drawEllipse(cx - knobR, cy - knobR, knobR * 2.0f, knobR * 2.0f, 1.0f);

    // Indicator dot
    float dotDist = knobR * 0.65f;
    float dotX = cx + dotDist * std::sin(angle);
    float dotY = cy - dotDist * std::cos(angle);
    g.setColour(getNeonCyan());
    g.fillEllipse(dotX - 2.5f, dotY - 2.5f, 5.0f, 5.0f);
    // Dot glow
    g.setColour(getNeonCyan().withAlpha(0.3f));
    g.fillEllipse(dotX - 5.0f, dotY - 5.0f, 10.0f, 10.0f);
}

void TB303LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& /*backgroundColour*/,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

    // Dark button with subtle border
    juce::Colour bgCol = shouldDrawButtonAsDown ? getBgLight().brighter(0.15f)
                        : shouldDrawButtonAsHighlighted ? getBgLight().brighter(0.08f)
                        : getBgLight();

    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 4.0f);

    juce::Colour borderCol = shouldDrawButtonAsDown ? getNeonCyan().withAlpha(0.5f) : getBorderLight();
    g.setColour(borderCol);
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    if (shouldDrawButtonAsDown)
    {
        g.setColour(getNeonCyan().withAlpha(0.08f));
        g.fillRoundedRectangle(bounds, 4.0f);
    }
}

void TB303LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    bool on = button.getToggleState();
    juce::Colour bgCol = on ? getNeonCyan().withAlpha(0.15f) : getBgLight();
    if (shouldDrawButtonAsHighlighted)
        bgCol = bgCol.brighter(0.05f);

    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(on ? getNeonCyan().withAlpha(0.6f) : getBorderLight());
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    g.setColour(on ? getNeonCyan() : getTextDim());
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 2);
}

void TB303LookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                     bool /*isButtonDown*/, int /*buttonX*/, int /*buttonY*/,
                                     int /*buttonW*/, int /*buttonH*/, juce::ComboBox& /*box*/)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    g.setColour(getBgLight());
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(getBorderLight());
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

    // Arrows
    float arrowX = static_cast<float>(width) - 16.0f;
    float midY = static_cast<float>(height) * 0.5f;
    juce::Path upArrow, downArrow;
    upArrow.addTriangle(arrowX, midY - 2.0f, arrowX + 8.0f, midY - 2.0f, arrowX + 4.0f, midY - 6.0f);
    downArrow.addTriangle(arrowX, midY + 2.0f, arrowX + 8.0f, midY + 2.0f, arrowX + 4.0f, midY + 6.0f);
    g.setColour(getNeonCyan());
    g.fillPath(upArrow);
    g.fillPath(downArrow);
}

void TB303LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(label.getFont());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         juce::jmax(1, static_cast<int>(static_cast<float>(textArea.getHeight()) / label.getFont().getHeight())),
                         label.getMinimumHorizontalScale());
    }
}

void TB303LookAndFeel::drawFuturisticPanel(juce::Graphics& g, juce::Rectangle<float> bounds,
                                             juce::Colour accentColor)
{
    // Dark background
    g.setColour(getPanelBg());
    g.fillRoundedRectangle(bounds, 6.0f);

    // Subtle border with accent glow
    g.setColour(getBorder());
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    // Top accent line
    g.setColour(accentColor.withAlpha(0.3f));
    g.drawLine(bounds.getX() + 20.0f, bounds.getY() + 0.5f,
               bounds.getRight() - 20.0f, bounds.getY() + 0.5f, 1.5f);

    // Glow at top
    juce::ColourGradient glow(accentColor.withAlpha(0.06f), bounds.getCentreX(), bounds.getY(),
                               juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getY() + 30.0f, false);
    g.setGradientFill(glow);
    g.fillRect(bounds.withHeight(30.0f));
}

void TB303LookAndFeel::drawGlowLine(juce::Graphics& g, float x1, float y1, float x2, float y2,
                                       juce::Colour color, float thickness)
{
    // Glow
    g.setColour(color.withAlpha(0.15f));
    g.drawLine(x1, y1, x2, y2, thickness + 4.0f);
    // Core line
    g.setColour(color.withAlpha(0.4f));
    g.drawLine(x1, y1, x2, y2, thickness);
}
