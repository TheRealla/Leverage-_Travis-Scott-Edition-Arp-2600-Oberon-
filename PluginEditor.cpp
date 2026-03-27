// PluginEditor.cpp
#include "PluginEditor.h"

ARPoberon2608AudioProcessorEditor::ARPoberon2608AudioProcessorEditor(ARPoberon2608AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1280, 720);  // Matches the image proportions

    // Title
    titleLabel.setText("ARP-OBERON 2608", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::gold);
    addAndMakeVisible(titleLabel);

    // === Bottom MASTER Section (exact match to image) ===
    preampSlider.setSliderStyle(juce::Slider::Rotary);
    preampSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    preampSlider.setRange(0.2, 6.0, 0.01);
    preampSlider.setValue(1.8);
    preampSlider.addListener(this);
    styleKnob(preampSlider, juce::Colours::orange);
    addAndMakeVisible(preampSlider);

    softLimiterSlider.setSliderStyle(juce::Slider::Rotary);
    softLimiterSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    softLimiterSlider.setRange(0.0, 1.0, 0.01);
    softLimiterSlider.setValue(0.75);
    softLimiterSlider.addListener(this);
    styleKnob(softLimiterSlider, juce::Colours::silver);
    addAndMakeVisible(softLimiterSlider);

    tapeSatSlider.setSliderStyle(juce::Slider::Rotary);
    tapeSatSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    tapeSatSlider.setRange(0.0, 1.0, 0.01);
    tapeSatSlider.setValue(0.6);
    tapeSatSlider.addListener(this);
    styleKnob(tapeSatSlider, juce::Colours::silver);
    addAndMakeVisible(tapeSatSlider);

    // Glowing Red DRIVE Button
    driveButton.setButtonText("DRIVE");
    driveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(180, 20, 20));
    driveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    driveButton.setToggleState(false, juce::dontSendNotification);
    driveButton.addListener(this);
    addAndMakeVisible(driveButton);

    // Filter controls (Oberon Ladder)
    filterCutoff.setSliderStyle(juce::Slider::Rotary);
    filterCutoff.setRange(30.0, 18000.0, 1.0);
    filterCutoff.setValue(1400.0);
    filterCutoff.addListener(this);
    styleKnob(filterCutoff, juce::Colours::gold);
    addAndMakeVisible(filterCutoff);

    filterRes.setSliderStyle(juce::Slider::Rotary);
    filterRes.setRange(0.0, 20.0, 0.01);
    filterRes.setValue(4.5);
    filterRes.addListener(this);
    styleKnob(filterRes, juce::Colours::gold);
    addAndMakeVisible(filterRes);

    filterMode.addItem("LP", 1);
    filterMode.addItem("HP", 2);
    filterMode.addItem("All-Pass", 3);
    filterMode.setSelectedId(1);
    filterMode.addListener(this);
    addAndMakeVisible(filterMode);

    // Attach parameters (link to APVTS)
    auto& apvts = processor.apvts;
    preampAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "preamp", preampSlider);
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filter_cutoff", filterCutoff);
    resAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "filter_res", filterRes);

    // TODO: Add more attachments for Osc A/B, Tape, etc.
}

void ARPoberon2608AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Wooden frame background (dark walnut like the image)
    g.fillAll(juce::Colour(30, 25, 20));  // Dark wood base

    // Main panel
    juce::Rectangle<int> panel = getLocalBounds().reduced(20, 40);
    g.setColour(juce::Colour(45, 40, 35));
    g.fillRoundedRectangle(panel.toFloat(), 12.0f);

    // Brass/gold accents
    g.setColour(juce::Colours::gold.withAlpha(0.15f));
    g.drawRoundedRectangle(panel.toFloat(), 12.0f, 4.0f);

    // Title area
    g.setColour(juce::Colours::black);
    g.fillRect(0, 0, getWidth(), 60);
}

void ARPoberon2608AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(30);

    // Top title
    titleLabel.setBounds(bounds.removeFromTop(50).reduced(20, 0));

    // Bottom MASTER section (matches image layout)
    auto masterArea = bounds.removeFromBottom(140);
    preampSlider.setBounds(masterArea.removeFromLeft(120).reduced(10));
    softLimiterSlider.setBounds(masterArea.removeFromLeft(120).reduced(10));
    tapeSatSlider.setBounds(masterArea.removeFromLeft(120).reduced(10));

    driveButton.setBounds(masterArea.removeFromRight(100).reduced(15, 30));
    driveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(255, 60, 60)); // glowing red when on

    // Filter controls (center-right)
    auto filterArea = bounds.removeFromRight(300);
    filterCutoff.setBounds(filterArea.removeFromTop(100).reduced(20));
    filterRes.setBounds(filterArea.removeFromTop(100).reduced(20));
    filterMode.setBounds(filterArea.removeFromTop(60).reduced(30, 10));

    // Left ARP section would go here (sliders for VCO 1-3, patch points)
    // Right Oberon section for Osc A/B and Filter 2
}

void ARPoberon2608AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    updateParametersFromUI();
}

void ARPoberon2608AudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &driveButton)
    {
        bool isOn = driveButton.getToggleState();
        // Link to "secret_drive" parameter if you added it
        driveButton.setColour(juce::TextButton::buttonColourId, 
                              isOn ? juce::Colour(255, 80, 80) : juce::Colour(180, 20, 20));
    }
}

void ARPoberon2608AudioProcessorEditor::styleKnob(juce::Slider& slider, juce::Colour colour)
{
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::silver);
}

void ARPoberon2608AudioProcessorEditor::updateParametersFromUI()
{
    // Sync any manual changes if needed
}    // Sync any manual changes if needed
}
