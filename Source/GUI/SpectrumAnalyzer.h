/*
  ==============================================================================

    SpectrumAnalyzer.h
    Created: 9 Jan 2022 12:05:28pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PathProducer.h"

struct SpectrumAnalyzer : juce::Component,
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
    SpectrumAnalyzer(SimpleMBCompAudioProcessor&);
    ~SpectrumAnalyzer();

    void parameterValueChanged(int parameterIndex, float NewValue) override;
    void parameterGestureChanged(int parameterindex, bool gestureIsStarting) override {};
    void timerCallback() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void toggleAnalysisEnablement(bool enabled);
private:
    SimpleMBCompAudioProcessor& audioProcessor;

    juce::Atomic<bool> parametersChanged{ false };

    //monoChain monoChain;

    //void updateChain();

    juce::Image background;

    juce::Rectangle<int> getRenderArea();

    juce::Rectangle<int> getAnalysisArea();

    PathProducer leftPathProducer, rightPathProducer;

    bool shouldShowFFTAnalysis = true;
};