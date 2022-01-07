/*
  ==============================================================================

    utilityComponents.cpp
    Created: 7 Jan 2022 4:45:58pm
    Author:  erwan

  ==============================================================================
*/

#include "utilityComponents.h"

//==============================================================================
Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

void Placeholder::paint(juce::Graphics& g)
{
    g.fillAll(customColor);
}

//==============================================================================
RotarySlider::RotarySlider() :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
{
}