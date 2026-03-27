// PluginProcessor.cpp
#include "PluginProcessor.h"
#include "CRBOberonDSP.h"

ARPoberon2608AudioProcessor::ARPoberon2608AudioProcessor()
     : AudioProcessor (BusesProperties()
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

ARPoberon2608AudioProcessor::~ARPoberon2608AudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
ARPoberon2608AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Bottom MASTER section (matches your GUI image)
    layout.add (std::make_unique<juce::AudioParameterFloat> ("preamp", "PREAMP/DRIVE",
        juce::NormalisableRange<float> (0.2f, 6.0f, 0.01f), 1.8f));

    // Oberon Filter Ladder parameters
    layout.add (std::make_unique<juce::AudioParameterFloat> ("filter_cutoff", "Cutoff",
        juce::NormalisableRange<float> (30.0f, 18000.0f, 1.0f), 1400.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("filter_res", "Resonance",
        juce::NormalisableRange<float> (0.0f, 20.0f, 0.01f), 4.5f));

    layout.add (std::make_unique<juce::AudioParameterInt> ("filter_mode", "Filter Mode", 
        0, 2, 0)); // 0 = LP, 1 = HP, 2 = All-Pass

    // Red DRIVE button (engages full secret chain + rectifier modulation)
    layout.add (std::make_unique<juce::AudioParameterBool> ("secret_drive", "DRIVE", false));

    // Patch Bay parameters (simple for now - can be expanded)
    layout.add (std::make_unique<juce::AudioParameterInt> ("patch_filter_cutoff", "Patch to Filter Cutoff", 
        0, 4, 0)); // 0=None, 1=Rectifier, 2=Noise, 3=Env1, 4=VCO3

    layout.add (std::make_unique<juce::AudioParameterInt> ("patch_am_depth", "Patch to AM Depth", 
        0, 4, 0));

    return layout;
}

void ARPoberon2608AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oberonDSP.prepare(sampleRate);
}

void ARPoberon2608AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // === MIDI Note Handling (monophonic for historical Oberon character) ===
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            oberonDSP.noteOn (juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber()));
        else if (msg.isNoteOff())
            oberonDSP.noteOff();
    }

    // === Read Parameters from GUI ===
    float preampValue       = *apvts.getRawParameterValue ("preamp");
    float filterCutoffValue = *apvts.getRawParameterValue ("filter_cutoff");
    float filterResValue    = *apvts.getRawParameterValue ("filter_res");
    int   filterModeValue   = *apvts.getRawParameterValue ("filter_mode");
    bool  secretDrive       = *apvts.getRawParameterValue ("secret_drive");

    int patchFilterCutoff   = *apvts.getRawParameterValue ("patch_filter_cutoff");
    int patchAMDepth        = *apv
