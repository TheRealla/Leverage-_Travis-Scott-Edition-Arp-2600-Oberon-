// PluginEditor.h
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ARPoberon2608AudioProcessorEditor : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener,
                                          public juce::Button::Listener
{
public:
    ARPoberon2608AudioProcessorEditor(ARPoberon2608AudioProcessor&);
    ~ARPoberon2608AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    ARPoberon2608AudioProcessor& processor;

    // Background and styling
    juce::Image woodBackground;

    // Left ARP Section (VCOs, VCF, ENV, Patch points)
    juce::Slider vco1Freq, vco1Level, vco2Freq, vco2Level, vco3Freq, vco3Level;
    juce::Slider filterCutoff, filterRes;
    juce::ComboBox filterMode;

    // Right Oberon Section
    juce::Slider oscAPitch, oscBSpread, oscAShape;
    juce::Slider filter2Cutoff, filter2Res, filter2Drive;

    // Bottom MASTER Section (matches image)
    juce::Slider preampSlider, softLimiterSlider, tapeSatSlider;
    juce::TextButton driveButton;           // Glowing red DRIVE button
    juce::Slider vuMeter;                   // Simulated VU

    juce::Label titleLabel;

    void updateParametersFromUI();
    void styleKnob(juce::Slider& slider, juce::Colour colour);
};
