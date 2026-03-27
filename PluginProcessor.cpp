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

    // Bottom MASTER section from the image
    layout.add (std::make_unique<juce::AudioParameterFloat> ("preamp", "PREAMP/DRIVE",
        juce::NormalisableRange<float> (0.2f, 6.0f, 0.01f), 1.8f));

    // Oberon Filter Ladder
    layout.add (std::make_unique<juce::AudioParameterFloat> ("filter_cutoff", "Cutoff",
        juce::NormalisableRange<float> (30.0f, 18000.0f, 1.0f), 1400.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("filter_res", "Resonance",
        juce::NormalisableRange<float> (0.0f, 20.0f, 0.01f), 4.5f));

    layout.add (std::make_unique<juce::AudioParameterInt> ("filter_mode", "Filter Mode", 0, 2, 0)); // 0=LP, 1=HP, 2=All-Pass

    // Red DRIVE button
    layout.add (std::make_unique<juce::AudioParameterBool> ("secret_drive", "DRIVE", false));

    return layout;
}

void ARPoberon2608AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oberonDSP.prepare(sampleRate);
}

void ARPoberon2608AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Handle MIDI notes (basic monophonic for now)
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            oberonDSP.noteOn (juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber()));
        else if (msg.isNoteOff())
            oberonDSP.noteOff();
    }

    // Get current parameters
    float preampValue = *apvts.getRawParameterValue ("preamp");
    oberonDSP.setPreamp (preampValue);

    // Process audio (mono for Oberon character, then duplicate to stereo)
    auto* leftChannel = buffer.getWritePointer (0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        leftChannel[i] = oberonDSP.process();
    }

    // Copy to right channel
    if (buffer.getNumChannels() > 1)
        buffer.copyFrom (1, 0, buffer, 0, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* ARPoberon2608AudioProcessor::createEditor()
{
    return new ARPoberon2608AudioProcessorEditor (*this);
}

bool ARPoberon2608AudioProcessor::hasEditor() const { return true; }

const juce::String ARPoberon2608AudioProcessor::getName() const { return "ARP-OBERON 2608"; }

bool ARPoberon2608AudioProcessor::acceptsMidi() const { return true; }
bool ARPoberon2608AudioProcessor::producesMidi() const { return false; }
bool ARPoberon2608AudioProcessor::isMidiEffect() const { return false; }
double ARPoberon2608AudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ARPoberon2608AudioProcessor::getNumPrograms() { return 1; }
int ARPoberon2608AudioProcessor::getCurrentProgram() { return 0; }
void ARPoberon2608AudioProcessor::setCurrentProgram (int) {}
const juce::String ARPoberon2608AudioProcessor::getProgramName (int) { return {}; }
void ARPoberon2608AudioProcessor::changeProgramName (int, const juce::String&) {}

void ARPoberon2608AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ARPoberon2608AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// This creates the processor instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ARPoberon2608AudioProcessor();
}
