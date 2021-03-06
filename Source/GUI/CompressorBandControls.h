/*
  ==============================================================================

    CompressorBandControls.h
    Created: 7 Jan 2022 4:56:18pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../DSP/Params.h"
#include "RotarySliderWithLabels.h"

//==============================================================================
/**
*/
struct CompressorBandControls : juce::Component, juce::Button::Listener
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorBandControls() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void buttonClicked(juce::Button* button) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    using RSWL = RotarySliderWithLabels;
    RSWL attackSlider, releaseSlider, thresholdSlider;

    RatioSlider ratioSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment, releaseSliderAttachment, thresholdSliderAttachment, ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;

    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    juce::ToggleButton* activeBand = &lowBand;

    void updateAttachments();

    void updateSliderEnablement();

    void updateSoloMuteBypassToggleState(juce::Button& clickedButton);

    void updateActiveBandFillColors(juce::Button& clickedButton);

    void resetActiveBandColors();

    static void refreshBandButtonColors(juce::Button& band, juce::Button& colorSource);

    void updateBandSelectButtonState();
};
