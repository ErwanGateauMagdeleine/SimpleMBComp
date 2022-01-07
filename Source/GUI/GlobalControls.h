/*
  ==============================================================================

    GlobalControls.h
    Created: 7 Jan 2022 4:59:05pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"
#include "../DSP/Params.h"

//==============================================================================
/**
*/
struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr< Attachment> inGainSliderAttachment, lowMidXoverSliderAttachment, midHighXoverSliderAttachment, outGainSliderAttachment;
};