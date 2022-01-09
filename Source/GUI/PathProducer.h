/*
  ==============================================================================

    PathProducer.h
    Created: 9 Jan 2022 12:05:49pm
    Author:  erwan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../DSP/SingleChannelSampleFifo.h"
#include "AnalyzerPathGenerator.h"
#include "FFTDataGenerator.h"
#include "Utilities.h"
#include "../PluginProcessor.h"

struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>& scsf) :
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; };

private:
    SingleChannelSampleFifo<SimpleMBCompAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;
};