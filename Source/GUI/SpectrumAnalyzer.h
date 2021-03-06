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

    std::vector<float> getGains();
    std::vector<float> getFrequencies();
    std::vector<float> getXs(const std::vector<float>& freqs, float left, float width);

    //void drawBackGroundGrid(juce::Graphics& g);
    void drawBackGroundGrid(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTextLabels(juce::Graphics& g, juce::Rectangle<int> bounds);
private:
    SimpleMBCompAudioProcessor& audioProcessor;

    juce::Atomic<bool> parametersChanged{ false };

    //monoChain monoChain;

    //void updateChain();

    juce::Image background;

    juce::Rectangle<int> getRenderArea(juce::Rectangle<int> bounds);

    juce::Rectangle<int> getAnalysisArea(juce::Rectangle<int> bounds);

    PathProducer leftPathProducer, rightPathProducer;

    bool shouldShowFFTAnalysis = true;

    void drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds);

    void drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds);

    juce::AudioParameterFloat* lowMidXoverParam{ nullptr };
    juce::AudioParameterFloat* midHighXoverParam{ nullptr };

    juce::AudioParameterFloat* lowThresholdParam{ nullptr };
    juce::AudioParameterFloat* midThresholdParam{ nullptr };
    juce::AudioParameterFloat* highThresholdParam{ nullptr };
};