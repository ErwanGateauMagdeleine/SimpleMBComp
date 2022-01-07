/*
  ==============================================================================

    utilityComponents.h
    Created: 7 Jan 2022 4:45:58pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================
/**
*/
struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override;

    juce::Colour customColor;
};

//==============================================================================
/**
*/
struct RotarySlider : juce::Slider
{
    RotarySlider();
};