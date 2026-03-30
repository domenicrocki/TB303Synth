#include "TB303LookAndFeel.h"

TB303LookAndFeel::TB303LookAndFeel()
{
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, textDk());
    setColour(juce::ComboBox::backgroundColourId, dispBg());
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF808080));
    setColour(juce::ComboBox::textColourId, textDk());
    setColour(juce::PopupMenu::backgroundColourId, dispBg());
    setColour(juce::PopupMenu::textColourId, textDk());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF4060A0));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void TB303LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                         float pos, float startA, float endA, juce::Slider&)
{
    auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat().reduced(2.0f);
    auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
    auto angle = startA + pos * (endA - startA);
    paintMetalKnob(g, cx, cy, r, angle);
}

void TB303LookAndFeel::paintMetalKnob(juce::Graphics& g, float cx, float cy, float r, float angle)
{
    // Shadow
    g.setColour(juce::Colour(0x30000000));
    g.fillEllipse(cx - r + 1, cy - r + 2, r * 2, r * 2);

    // Outer ring
    {
        juce::ColourGradient gr(juce::Colour(0xFFE8E8EC), cx - r * 0.6f, cy - r * 0.6f,
                                 juce::Colour(0xFF808088), cx + r * 0.6f, cy + r * 0.6f, true);
        g.setGradientFill(gr);
        g.fillEllipse(cx - r, cy - r, r * 2, r * 2);
    }
    g.setColour(juce::Colour(0xFF606068));
    g.drawEllipse(cx - r, cy - r, r * 2, r * 2, 1.2f);

    // Inner cap
    auto ir = r * 0.72f;
    {
        juce::ColourGradient gr(juce::Colour(0xFFD8D8E0), cx - ir * 0.4f, cy - ir * 0.4f,
                                 juce::Colour(0xFF909098), cx + ir * 0.4f, cy + ir * 0.4f, true);
        g.setGradientFill(gr);
        g.fillEllipse(cx - ir, cy - ir, ir * 2, ir * 2);
    }

    // Indicator line
    auto len = r * 0.85f;
    auto thick = juce::jmax(1.8f, r * 0.08f);
    juce::Path ind;
    ind.addRoundedRectangle(-thick * 0.5f, -len, thick, len * 0.4f, 1.0f);
    g.setColour(juce::Colours::white);
    g.fillPath(ind, juce::AffineTransform::rotation(angle).translated(cx, cy));

    g.setColour(juce::Colour(0xFF505058));
    g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);
}

void TB303LookAndFeel::paintLargeKnob(juce::Graphics& g, float cx, float cy, float r,
                                        float pos, float startA, float endA)
{
    auto angle = startA + pos * (endA - startA);
    g.setColour(juce::Colour(0xFF606068));
    g.drawEllipse(cx - r, cy - r, r * 2, r * 2, 2);

    int ticks = 31;
    for (int i = 0; i < ticks; ++i)
    {
        float ta = startA + (float(i) / float(ticks - 1)) * (endA - startA);
        float ri = r - 4, ro = r;
        g.setColour(juce::Colour(0xFF505058));
        g.drawLine(cx + ri * std::sin(ta), cy - ri * std::cos(ta),
                   cx + ro * std::sin(ta), cy - ro * std::cos(ta), 0.7f);
    }

    auto kr = r * 0.8f;
    {
        juce::ColourGradient gr(juce::Colour(0xFFE0E0E8), cx - kr * 0.5f, cy - kr * 0.5f,
                                 juce::Colour(0xFF8A8A92), cx + kr * 0.5f, cy + kr * 0.5f, true);
        g.setGradientFill(gr);
        g.fillEllipse(cx - kr, cy - kr, kr * 2, kr * 2);
    }

    juce::Path ind;
    ind.addRoundedRectangle(-1.5f, -kr * 0.88f, 3, kr * 0.35f, 1);
    g.setColour(juce::Colours::white);
    g.fillPath(ind, juce::AffineTransform::rotation(angle).translated(cx, cy));
}

void TB303LookAndFeel::paintLED(juce::Graphics& g, float x, float y, float sz, bool on)
{
    if (on)
    {
        g.setColour(ledOn().withAlpha(0.25f));
        g.fillEllipse(x - 2, y - 2, sz + 4, sz + 4);
    }
    g.setColour(on ? ledOn() : ledOff());
    g.fillEllipse(x, y, sz, sz);
}

void TB303LookAndFeel::paint3DButton(juce::Graphics& g, juce::Rectangle<float> b, bool pressed)
{
    g.setColour(pressed ? silverDk() : silver());
    g.fillRoundedRectangle(b, 2);
    g.setColour(pressed ? juce::Colour(0xFF909098) : silverLt());
    g.drawLine(b.getX() + 1, b.getY() + 1, b.getRight() - 1, b.getY() + 1, 1);
    g.drawLine(b.getX() + 1, b.getY() + 1, b.getX() + 1, b.getBottom() - 1, 1);
    g.setColour(pressed ? silverLt() : juce::Colour(0xFF808088));
    g.drawLine(b.getX() + 1, b.getBottom() - 1, b.getRight() - 1, b.getBottom() - 1, 1);
    g.drawLine(b.getRight() - 1, b.getY() + 1, b.getRight() - 1, b.getBottom() - 1, 1);
}

void TB303LookAndFeel::paintSilverBg(juce::Graphics& g, juce::Rectangle<float> b, bool darker)
{
    auto top = darker ? juce::Colour(0xFFCCCCD0) : juce::Colour(0xFFDADADE);
    auto bot = darker ? juce::Colour(0xFFBABABE) : juce::Colour(0xFFC6C6CA);
    juce::ColourGradient gr(top, 0, b.getY(), bot, 0, b.getBottom(), false);
    g.setGradientFill(gr);
    g.fillRect(b);
    g.setColour(silverLt());
    g.drawLine(b.getX(), b.getY(), b.getRight(), b.getY(), 1);
    g.setColour(juce::Colour(0xFFA0A0A8));
    g.drawLine(b.getX(), b.getBottom() - 1, b.getRight(), b.getBottom() - 1, 1);
}

void TB303LookAndFeel::paintDarkBg(juce::Graphics& g, juce::Rectangle<float> b)
{
    g.setColour(panelDk());
    g.fillRect(b);
    auto inner = b.reduced(5);
    g.setColour(panelMd());
    g.fillRoundedRectangle(inner, 5);
    g.setColour(juce::Colour(0xFF1E1E24));
    g.drawRoundedRectangle(inner, 5, 1);
}

void TB303LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button&,
                                              const juce::Colour&, bool, bool down)
{
    auto b = g.getClipBounds().toFloat().reduced(0.5f);
    paint3DButton(g, b, down);
}

void TB303LookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool hi, bool)
{
    auto b = button.getLocalBounds().toFloat().reduced(1);
    auto on = button.getToggleState();
    g.setColour(on ? silverDk() : silver());
    if (hi) g.setColour(on ? silverDk().brighter(0.05f) : silver().brighter(0.05f));
    g.fillRoundedRectangle(b, 2);
    g.setColour(juce::Colour(0xFF707078));
    g.drawRoundedRectangle(b, 2, 1);
    g.setColour(textDk());
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(2),
                     juce::Justification::centred, 2);
}

void TB303LookAndFeel::drawComboBox(juce::Graphics& g, int w, int h,
                                     bool, int, int, int, int, juce::ComboBox&)
{
    auto b = juce::Rectangle<float>(0, 0, (float)w, (float)h);
    g.setColour(dispBg());
    g.fillRoundedRectangle(b, 3);
    g.setColour(juce::Colour(0xFF707078));
    g.drawRoundedRectangle(b.reduced(0.5f), 3, 1);
    float ax = (float)w - 14, my = (float)h * 0.5f;
    juce::Path up, dn;
    up.addTriangle(ax, my - 2, ax + 7, my - 2, ax + 3.5f, my - 6);
    dn.addTriangle(ax, my + 2, ax + 7, my + 2, ax + 3.5f, my + 6);
    g.setColour(juce::Colour(0xFF404048));
    g.fillPath(up); g.fillPath(dn);
}
