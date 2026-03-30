#include "KnobComponent.h"

KnobComponent::KnobComponent(const juce::String& labelText, const juce::String& paramId,
                             juce::AudioProcessorValueTreeState& apvts)
{
    slider_.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider_.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider_.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(slider_);

    label_.setText(labelText, juce::dontSendNotification);
    label_.setJustificationType(juce::Justification::centred);
    label_.setFont(juce::Font(10.0f, juce::Font::bold));
    label_.setColour(juce::Label::textColourId, juce::Colour(0xFF1A1A1A));
    addAndMakeVisible(label_);

    attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramId, slider_);
}

KnobComponent::~KnobComponent() {}

void KnobComponent::resized()
{
    auto bounds = getLocalBounds();
    int labelHeight = 16;

    if (labelBelow_)
    {
        slider_.setBounds(bounds.removeFromTop(bounds.getHeight() - labelHeight));
        label_.setBounds(bounds);
    }
    else
    {
        label_.setBounds(bounds.removeFromTop(labelHeight));
        slider_.setBounds(bounds);
    }
}
