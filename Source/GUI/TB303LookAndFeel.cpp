#include "TB303LookAndFeel.h"

TB303LookAndFeel::TB303LookAndFeel()
{
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, TB303Colors::textBright());
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF1A1A28));
    setColour(juce::ComboBox::outlineColourId, TB303Colors::panelBorder());
    setColour(juce::ComboBox::textColourId, TB303Colors::textBright());
    setColour(juce::ComboBox::arrowColourId, TB303Colors::textDim());
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF1A1A28));
    setColour(juce::PopupMenu::textColourId, TB303Colors::textBright());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, TB303Colors::cyan().withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1E1E2E));
    setColour(juce::TextButton::textColourOffId, TB303Colors::textBright());
    setColour(juce::TextButton::textColourOnId, TB303Colors::cyan());
}

void TB303LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                         float pos, float startA, float endA, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat().reduced(4.0f);
    auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
    auto angle = startA + pos * (endA - startA);

    juce::Colour arcColor = TB303Colors::cyan();
    auto* prop = slider.getProperties().getVarPointer("arcColor");
    if (prop != nullptr && prop->isInt64())
        arcColor = juce::Colour(static_cast<juce::uint32>(static_cast<juce::int64>(*prop)));

    float trackThickness = juce::jmax(2.5f, r * 0.12f);
    g.setColour(TB303Colors::knobTrack());
    juce::Path trackArc;
    trackArc.addCentredArc(cx, cy, r - 2.0f, r - 2.0f, 0.0f, startA, endA, true);
    g.strokePath(trackArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

    if (pos > 0.001f)
    {
        g.setColour(arcColor);
        juce::Path activeArc;
        activeArc.addCentredArc(cx, cy, r - 2.0f, r - 2.0f, 0.0f, startA, angle, true);
        g.strokePath(activeArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    float knobR = r * 0.65f;
    g.setColour(TB303Colors::knobBody());
    g.fillEllipse(cx - knobR, cy - knobR, knobR * 2, knobR * 2);

    g.setColour(TB303Colors::panelBorder());
    g.drawEllipse(cx - knobR, cy - knobR, knobR * 2, knobR * 2, 1.0f);

    float thick = juce::jmax(2.0f, r * 0.08f);
    float lineLen = knobR * 0.8f;
    juce::Path ind;
    ind.addRoundedRectangle(-thick * 0.5f, -knobR + 2.0f, thick, lineLen, 1.0f);
    g.setColour(arcColor);
    g.fillPath(ind, juce::AffineTransform::rotation(angle).translated(cx, cy));
}

void TB303LookAndFeel::drawComboBox(juce::Graphics& g, int w, int h,
                                     bool, int, int, int, int, juce::ComboBox&)
{
    auto b = juce::Rectangle<float>(0, 0, (float)w, (float)h);
    g.setColour(juce::Colour(0xFF1A1A28));
    g.fillRoundedRectangle(b, 4.0f);
    g.setColour(TB303Colors::panelBorder());
    g.drawRoundedRectangle(b.reduced(0.5f), 4.0f, 1.0f);

    float ax = (float)w - 16, my = (float)h * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(ax, my - 3, ax + 8, my - 3, ax + 4, my + 3);
    g.setColour(TB303Colors::textDim());
    g.fillPath(arrow);
}

void TB303LookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    g.fillAll(juce::Colour(0xFF1A1A28));
    g.setColour(TB303Colors::panelBorder());
    g.drawRect(0, 0, width, height, 1);
}

void TB303LookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted,
                                          bool isTicked, bool /*hasSubMenu*/,
                                          const juce::String& text, const juce::String& /*shortcutText*/,
                                          const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        g.setColour(TB303Colors::panelBorder());
        g.fillRect(area.reduced(5, 0).withHeight(1).withCentre(area.getCentre()));
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(TB303Colors::cyan().withAlpha(0.2f));
        g.fillRect(area);
    }

    g.setColour(isActive ? TB303Colors::textBright() : TB303Colors::textDim());
    g.setFont(juce::Font(13.0f));
    auto textArea = area.reduced(10, 0);
    g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);

    if (isTicked)
    {
        g.setColour(TB303Colors::cyan());
        g.fillEllipse(static_cast<float>(area.getRight() - 16), static_cast<float>(area.getCentreY() - 3), 6.0f, 6.0f);
    }
}

void TB303LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour&, bool isHighlighted, bool isDown)
{
    auto b = button.getLocalBounds().toFloat().reduced(1.0f);
    auto baseColor = juce::Colour(0xFF1E1E2E);

    if (isDown)
        baseColor = baseColor.brighter(0.15f);
    else if (isHighlighted)
        baseColor = baseColor.brighter(0.08f);

    if (button.getToggleState())
        baseColor = baseColor.brighter(0.1f);

    g.setColour(baseColor);
    g.fillRoundedRectangle(b, 4.0f);
    g.setColour(TB303Colors::panelBorder());
    g.drawRoundedRectangle(b, 4.0f, 1.0f);
}

void TB303LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool isHighlighted, bool /*isDown*/)
{
    auto b = button.getLocalBounds().toFloat().reduced(1.0f);
    auto on = button.getToggleState();

    auto baseColor = on ? juce::Colour(0xFF2A2A3E) : juce::Colour(0xFF1E1E2E);
    if (isHighlighted) baseColor = baseColor.brighter(0.06f);

    g.setColour(baseColor);
    g.fillRoundedRectangle(b, 4.0f);
    g.setColour(on ? TB303Colors::cyan() : TB303Colors::panelBorder());
    g.drawRoundedRectangle(b, 4.0f, 1.0f);

    g.setColour(on ? TB303Colors::textBright() : TB303Colors::textDim());
    g.setFont(juce::Font(11.0f));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(4),
                     juce::Justification::centred, 2);
}

void TB303LookAndFeel::paintSectionPanel(juce::Graphics& g, juce::Rectangle<int> bounds,
                                          const juce::String& title, juce::Colour accentColor)
{
    auto bf = bounds.toFloat();

    g.setColour(TB303Colors::panelBg());
    g.fillRoundedRectangle(bf, 4.0f);

    g.setColour(TB303Colors::panelBorder());
    g.drawRoundedRectangle(bf.reduced(0.5f), 4.0f, 1.0f);

    // Accent glow line at top
    g.setColour(accentColor);
    g.fillRect(bf.getX() + 4.0f, bf.getY() + 1.0f, bf.getWidth() - 8.0f, 2.0f);

    // Glow effect
    g.setColour(accentColor.withAlpha(0.15f));
    g.fillRect(bf.getX() + 2.0f, bf.getY(), bf.getWidth() - 4.0f, 8.0f);

    if (title.isNotEmpty())
    {
        g.setColour(accentColor);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText(title, bounds.getX() + 8, bounds.getY() + 5, bounds.getWidth() - 16, 14,
                   juce::Justification::centredLeft);
    }
}

void TB303LookAndFeel::paintLED(juce::Graphics& g, float x, float y, float sz, bool on)
{
    if (on)
    {
        g.setColour(TB303Colors::ledOn().withAlpha(0.3f));
        g.fillEllipse(x - 3, y - 3, sz + 6, sz + 6);
    }
    g.setColour(on ? TB303Colors::ledOn() : TB303Colors::ledOff());
    g.fillEllipse(x, y, sz, sz);

    if (on)
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillEllipse(x + sz * 0.2f, y + sz * 0.15f, sz * 0.35f, sz * 0.3f);
    }
}
