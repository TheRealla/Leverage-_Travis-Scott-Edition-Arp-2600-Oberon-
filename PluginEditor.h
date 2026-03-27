// PluginEditor.h
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OberonLookAndFeel : public juce::LookAndFeel_V4;

class ARPoberon2608AudioProcessorEditor : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener,
                                          public juce::Button::Listener
{
public:
    ARPoberon2608AudioProcessorEditor(ARPoberon2608AudioProcessor&);
    ~ARPoberon2608AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider*) override;
    void buttonClicked(juce::Button*) override;

private:
    ARPoberon2608AudioProcessor& processor;

    std::unique_ptr<OberonLookAndFeel> lookAndFeel;

    // Title
    juce::Label titleLabel;

    // Bottom MASTER
    juce::Slider preampSlider, softLimiterSlider, tapeSatSlider;
    juce::TextButton driveButton;

    // Oberon Filter Ladder
    juce::Slider filterCutoff, filterRes;
    juce::ComboBox filterMode;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> preampAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resAttachment;
};
