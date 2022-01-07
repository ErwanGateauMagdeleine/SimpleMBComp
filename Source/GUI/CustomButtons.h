/*
  ==============================================================================

    CustomButtons.h
    Created: 7 Jan 2022 4:42:58pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================
/**
*/
struct powerButton : juce::ToggleButton {};

//==============================================================================
/**
*/
struct analyzerButton : juce::ToggleButton
{
    void resized() override;

    juce::Path randomPath;
};
