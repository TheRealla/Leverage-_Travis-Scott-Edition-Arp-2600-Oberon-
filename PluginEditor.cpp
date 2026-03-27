// PluginEditor.cpp
#include "PluginEditor.h"
#include "PluginProcessor.h"

class OberonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OberonLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(255, 180, 60));   // brass/gold
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(60, 60, 60));
        setColour(juce::Slider::thumbColourId, juce::Colours::silver);
        setColour(juce::TextButton::buttonColourId, juce::Colour(180, 20, 20));         // red DRIVE
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(255, 80, 80));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto radius = juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;

        // Background circle
        g.setColour(juce::Colour(30, 30, 30));
        g.fillEllipse(rx, ry, rw, rw);

        // Metallic rim
        g.setColour(juce::Colour(80, 80, 80));
        g.drawEllipse(rx, ry, rw, rw, 6.0f);

        // Filled arc
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        juce::Path p;
        p.addCentredArc(centreX, centreY, radius - 6, radius - 6, 0.0f,
                        rotaryStartAngle, angle, true);

        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(p, juce::PathStrokeType(8.0f));

        // Knob center highlight
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillEllipse(centreX - 6, centreY - 6, 12, 12);
    }
};

ARPoberon2608AudioProcessorEditor::ARPoberon2608AudioProcessorEditor(ARPoberon2608AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1280, 720);

    // Custom LookAndFeel
    lookAndFeel = std::make_unique<OberonLookAndFeel>();
    juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get());

    // Title
    titleLabel.setText("ARP-OBERON 2608", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(34.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(255, 215, 100));
    addAndMakeVisible(titleLabel);

    // === Bottom MASTER Section (PREAMP/DRIVE, SOFT LIMITER, TAPE SATURATION) ===
    preampSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    preampSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    preampSlider.setRange(0.2, 6.0, 0.01);
    preampSlider.setValue(1.8);
    addAndMakeVisible(preampSlider);

    softLimiterSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    softLimiterSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    softLimiterSlider.setRange(0.0, 1.0, 0.01);
    softLimiterSlider.setValue(0.75);
    addAndMakeVisible(softLimiterSlider);

    tapeSatSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    tapeSatSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    tapeSatSlider.setRange(0.0, 1.0, 0.01);
    tapeSatSlider.setValue(0.6);
    addAndMakeVisible(tapeSatSlider);

    // Glowing Red DRIVE Button
    driveButton.setButtonText("DRIVE");
    driveButton.setToggleState(false, juce::dontSendNotification);
    driveButton.addListener(this);
    addAndMakeVisible(driveButton);

    // Oberon Filter Ladder controls
    filterCutoff.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoff.setRange(30.0, 18000.0, 1.0);
    filterCutoff.setValue(1400.0);
    addAndMakeVisible(filterCutoff);

    filterRes.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterRes.setRange(0.0, 20.0, 0.01);
    filterRes.setValue(4.5);
    addAndMakeVisible(filterRes);

    filterMode.addItem("LP", 1);
    filterMode.addItem("HP", 2);
    filterMode.addItem("All-Pass", 3);
    filterMode.setSelectedId(1);
    addAndMakeVisible(filterMode);

    // Attachments
    preampAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "preamp", preampSlider);
    cutoffAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "filter_cutoff", filterCutoff);
    resAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "filter_res", filterRes);
}

ARPoberon2608AudioProcessorEditor::~ARPoberon2608AudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void ARPoberon2608AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Dark walnut wood background
    g.fillAll(juce::Colour(25, 22, 18));

    // Main synth panel
    auto bounds = getLocalBounds().reduced(25, 35);
    g.setColour(juce::Colour(38, 34, 30));
    g.fillRoundedRectangle(bounds.toFloat(), 14.0f);

    // Subtle gold bevel
    g.setColour(juce::Colour(255, 215, 100).withAlpha(0.15f));
    g.drawRoundedRectangle(bounds.toFloat(), 14.0f, 3.0f);
}

void ARPoberon2608AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(35);

    titleLabel.setBounds(bounds.removeFromTop(55).reduced(40, 0));

    // Bottom MASTER section
    auto masterArea = bounds.removeFromBottom(170);
    int knobSize = 110;

    preampSlider.setBounds(masterArea.removeFromLeft(knobSize).reduced(15));
    softLimiterSlider.setBounds(masterArea.removeFromLeft(knobSize).reduced(15));
    tapeSatSlider.setBounds(masterArea.removeFromLeft(knobSize).reduced(15));

    // Red DRIVE button
    driveButton.setBounds(masterArea.removeFromRight(160).reduced(25, 45));

    // Filter 2 area (right side of image)
    auto rightArea = bounds.removeFromRight(340);
    filterCutoff.setBounds(rightArea.removeFromTop(130).reduced(30));
    filterRes.setBounds(rightArea.removeFromTop(130).reduced(30));
    filterMode.setBounds(rightArea.removeFromTop(70).reduced(50, 15));
}

void ARPoberon2608AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // Attachments handle syncing automatically
}

void ARPoberon2608AudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &driveButton)
    {
        bool isOn = driveButton.getToggleState();
        driveButton.setColour(juce::TextButton::buttonColourId,
                              isOn ? juce::Colour(255, 70, 70) : juce::Colour(180, 20, 20));
    }
}
