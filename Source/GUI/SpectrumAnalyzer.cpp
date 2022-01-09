/*
  ==============================================================================

    SpectrumAnalyzer.cpp
    Created: 9 Jan 2022 12:05:28pm
    Author:  erwan

  ==============================================================================
*/

#include "SpectrumAnalyzer.h"
#include "Utilities.h"

SpectrumAnalyzer::SpectrumAnalyzer(SimpleMBCompAudioProcessor& p) : audioProcessor(p),
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {   
        param->addListener(this);
    }

    const auto& paramNames = Params::GetParams();

    auto floatHelper = [&apvts = audioProcessor.apvts, &paramNames](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(paramNames.at(paramName)));
        jassert(param != nullptr);
    };

    floatHelper(lowMidXoverParam, Params::Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighXoverParam, Params::Names::Mid_High_Crossover_Freq);

    floatHelper(lowThresholdParam, Params::Names::Threshold_Low_Band);
    floatHelper(midThresholdParam, Params::Names::Threshold_Mid_Band);
    floatHelper(highThresholdParam, Params::Names::Threshold_High_Band);

    startTimerHz(60);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

std::vector<float> SpectrumAnalyzer::getGains()
{
    std::vector<float> values;
    auto increment = MAX_DECIBELS;

    for (auto db = NEGATIVE_INFINITY; db <= MAX_DECIBELS; db += increment)
    {
        values.push_back(db);
    }

    return values;
}

std::vector<float> SpectrumAnalyzer::getFrequencies()
{
    return std::vector<float>
    {
        20, /* 30, 40,*/ 50, 100,
        200, /* 300, 400,*/ 500, 1000,
        2000, /*3000, 4000,*/ 5000, 10000,
        20000
    };
}

float getXMapping(float frequency, float left, float width)
{
    auto normX = juce::mapFromLog10(frequency, MIN_FREQUENCY, MAX_FREQUENCY);

    return left + width * normX;
}

float getYMapping(float gain, float bottom, float top)
{
    return juce::jmap(gain, NEGATIVE_INFINITY, MAX_DECIBELS, bottom, top);
}

std::vector<float> SpectrumAnalyzer::getXs(const std::vector<float>& freqs, float left, float width)
{
    std::vector<float> xs;

    for (auto f : freqs)
    {
        xs.push_back(getXMapping(f, left, width));
    }

    return xs;
}

void SpectrumAnalyzer::drawBackGroundGrid(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;

    auto freqs = getFrequencies();

    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto xs = getXs(freqs, left, width);
    auto gain = getGains();

    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }

    for (auto gDb : gain)
    {
        auto y = getYMapping(gDb, float(bottom), float(top));

        g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}

void SpectrumAnalyzer::drawTextLabels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto freqs = getFrequencies();

    auto renderArea = getAnalysisArea(bounds);
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto xs = getXs(freqs, left, width);
    auto gain = getGains();

    g.setColour(juce::Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);

    for (int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        juce::String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000;
        }

        str << f;
        if (addK)
        {
            str << "k";
        }
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        juce::Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(bounds.getY());

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }

    for (auto gDb : gain)
    {
        auto y = juce::jmap(gDb, NEGATIVE_INFINITY, MAX_DECIBELS, float(bottom), float(top));

        juce::String str;
        if (gDb > 0)
        {
            str << "+";
        }
        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        juce::Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(bounds.getRight() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? juce::Colour(0u, 172u, 1u) : juce::Colours::lightgrey);

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);

        // str.clear();
        // str << (gDb - 24.f);

        r.setX(bounds.getX() + 1 );
        // textWidth = g.getCurrentFont().getStringWidth(str);
        // r.setSize(textWidth, fontHeight);
        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }

}

void SpectrumAnalyzer::drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;

    auto responseArea = getAnalysisArea(bounds);

    Graphics::ScopedSaveState sss(g);
    g.reduceClipRegion(responseArea);

    auto leftChannelFFTPath = leftPathProducer.getPath();
    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

    g.setColour(Colours::lightblue);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    auto rightChannelFFTPath = rightPathProducer.getPath();
    rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

    g.setColour(Colours::yellow);
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
}

void SpectrumAnalyzer::drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;

    bounds = getAnalysisArea(bounds);

    auto top = bounds.getY();
    const auto bottom = bounds.getBottom();
    const auto left = bounds.getX();
    const auto right = bounds.getRight();
    const auto width = bounds.getWidth();

    auto mapX = [left, width](float frequency)
    {
        return getXMapping(frequency, left, width);
    };

    auto lowMidX = mapX(lowMidXoverParam->get());
    g.setColour(Colours::orange);
    g.drawVerticalLine(lowMidX, top, bottom);

    auto midHighX = mapX(midHighXoverParam->get());
    g.drawVerticalLine(midHighX, top, bottom);

    auto mapY = [bottom, top](float gain)
    {
        return getYMapping(gain, bottom, top);
    };

    g.setColour(Colours::yellow);
    g.drawHorizontalLine(mapY(lowThresholdParam->get()),
                         left,
                         lowMidX);

    g.drawHorizontalLine(mapY(midThresholdParam->get()),
                         lowMidX,
                         midHighX);

    g.drawHorizontalLine(mapY(highThresholdParam->get()),
                         midHighX,
                         right);
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    using namespace juce;

    g.fillAll(Colours::black);

    auto bounds = drawModuleBackground(g, getLocalBounds());

    drawBackGroundGrid(g, bounds);

    if (shouldShowFFTAnalysis)
    {   
        drawFFTAnalysis(g, bounds);
    }

    drawCrossovers(g, bounds);

    drawTextLabels(g, bounds);

}

void SpectrumAnalyzer::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds();

    auto fftBounds = getAnalysisArea(bounds).toFloat();
    auto negInf = jmap(bounds.toFloat().getBottom(),
                       fftBounds.getBottom(), fftBounds.getY(),
                       NEGATIVE_INFINITY, MAX_DECIBELS);
    leftPathProducer.updateNegativeInfinity(negInf);
    rightPathProducer.updateNegativeInfinity(negInf);
}

void SpectrumAnalyzer::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

void SpectrumAnalyzer::toggleAnalysisEnablement(bool enabled)
{
    shouldShowFFTAnalysis = enabled;
}

void SpectrumAnalyzer::timerCallback()
{

    if (shouldShowFFTAnalysis)
    {
        auto bounds = getLocalBounds();
        auto fftBounds = getAnalysisArea(bounds).toFloat();
        fftBounds.setBottom(bounds.getBottom());
        auto sampleRate = audioProcessor.getSampleRate();

        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }

    if (parametersChanged.compareAndSetBool(false, true))
    {
    }

    repaint();
}

juce::Rectangle<int> SpectrumAnalyzer::getRenderArea(juce::Rectangle<int> bounds)
{
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}

juce::Rectangle<int> SpectrumAnalyzer::getAnalysisArea(juce::Rectangle<int> bounds)
{
    bounds = getRenderArea(bounds);
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
