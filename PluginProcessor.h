// PluginProcessor.h
#pragma once
#include <JuceHeader.h>
#include "CRBOberonDSP.h"   // We'll create this next

class ARPoberon2608AudioProcessor : public juce::AudioProcessor
{
public:
    ARPoberon2608AudioProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    // ... standard overrides

    juce::AudioProcessorValueTreeState apvts;

private:
    CRBOberon oberonDSP;   // Our historical Oberon core
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
