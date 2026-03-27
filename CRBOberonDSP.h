// CRBOberonDSP.h
#pragma once
#include <JuceHeader.h>
#include <vector>

// ===================================================================
// ARP 2600 LEFT SIDE - Semi-modular with Patch Bay + Rectifier
// ===================================================================

class OberonVCO {
public:
    OberonVCO() = default;
    void prepare(double sr) { sampleRate = sr; }
    void setFreq(double f) { freq = juce::jlimit(20.0, 20000.0, f); }
    void setPW(double p) { pw = juce::jlimit(0.01, 0.99, p); }
    
    double process() {
        phase += 2.0 * juce::MathConstants<double>::twoPi * freq / sampleRate;
        if (phase >= juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;
        
        double saw = (phase / juce::MathConstants<double>::twoPi) * 2.0 - 1.0;
        double sq = (phase < pw * juce::MathConstants<double>::twoPi) ? 1.0 : -1.0;
        double tri = 4.0 * std::fabs(saw) - 1.0;
        
        return (saw * 0.42 + sq * 0.29 + tri * 0.29) * 0.96; // Italian warmth
    }
private:
    double phase = 0.0, freq = 440.0, pw = 0.5;
    double sampleRate = 44100.0;
};

class ARP2600Rectifier {  // Classic ARP 2600 Envelope Follower / Audio Rectifier
public:
    void prepare(double sr) { sampleRate = sr; }
    double process(double input) {
        double rect = std::fabs(input);
        smoothed = smoothed * 0.96 + rect * 0.04;  // Musical smoothing
        return smoothed;
    }
    double getCV() const { return smoothed; }
private:
    double smoothed = 0.0;
    double sampleRate = 44100.0;
};

// ===================================================================
// OBERON RIGHT SIDE - Historical CRB Oberon Core
// ===================================================================

class OberonFilterLadder {
public:
    OberonFilterLadder() { updateCoeffs(); }
    void prepare(double sr) { sampleRate = sr; updateCoeffs(); }
    void setCutoff(double c) { cutoff = juce::jlimit(30.0, 18000.0, c); updateCoeffs(); }
    void setRes(double r) { res = juce::jlimit(0.0, 20.0, r); }
    void setMode(int m) { mode = m % 3; }
    
    double process(double input) {
        stage1 += alpha * (input - stage1);
        stage2 += alpha * (stage1 - stage2);
        stage3 += alpha * (stage2 - stage3);
        stage4 += alpha * (stage3 - stage4);
        
        double lp = stage4;
        double hp = input - (stage1 + stage2 + stage3 + stage4) * 0.25;
        double allpass = lp + hp;
        
        double feedback = res * lp * 0.85;
        
        switch (mode) {
            case 0: return lp - feedback;                    // Low-Pass
            case 1: return hp - feedback * 0.65;             // High-Pass
            case 2: return allpass * (1.0 - res * 0.35);     // All-Pass (unique third mode)
            default: return lp;
        }
    }
private:
    double stage1=0, stage2=0, stage3=0, stage4=0;
    double cutoff=1400.0, res=2.8, alpha=0.0;
    int mode=0;
    double sampleRate=44100.0;
    
    void updateCoeffs() {
        double wc = 2.0 * juce::MathConstants<double>::twoPi * cutoff / sampleRate;
        alpha = wc / (1.2 + wc * 0.55);
    }
};

class SoftLimiter {  // Exact amsynth hidden log/exp VCA-style limiter
public:
    double process(double x, double preamp = 1.0) {
        x *= preamp;
        if (x > xpeak) xpeak = 0.99 * xpeak + 0.01 * x;
        else xpeak = 0.998 * xpeak;
        
        double gain = 1.0;
        if (xpeak > 1e-8) {
            double val = std::log(xpeak) - std::log(0.9);
            if (val > 0.0) gain = std::exp(-val * 0.75);
        }
        return juce::jlimit(-1.0, 1.0, x * gain);
    }
private:
    double xpeak = 0.0;
};

// ===================================================================
// MAIN DSP CLASS WITH PATCH BAY
// ===================================================================

class CRBOberonDSP
{
public:
    enum PatchSource {
        None,
        RectifierCV,
        Noise,
        Env1,
        VCO3
    };

    CRBOberonDSP() = default;

    void prepare(double sampleRate);

    void noteOn(double freqHz, float velocity = 1.0f);
    void noteOff();

    float process();   // Main audio callback

    // Parameter setters
    void setPreamp(float value);
    void setFilterCutoff(float value);
    void setFilterRes(float value);
    void setFilterMode(int mode);
    void setRectifierEnabled(bool enabled);

    // Patch Bay routing
    void setFilterCutoffModSource(PatchSource source);
    void setAMDepthModSource(PatchSource source);

private:
    // Oscillators
    OberonVCO vco1, vco2, vco3;

    // Oberon Filter Ladder
    OberonFilterLadder filter;

    // ARP 2600 Rectifier (Left side)
    ARP2600Rectifier rectifier;
    bool rectifierEnabled = false;

    // amsynth Soft Limiter (VCA glue)
    SoftLimiter limiter;
    float preamp = 1.8f;

    // Patch Bay state
    PatchSource filterCutoffMod = PatchSource::None;
    PatchSource amDepthMod = PatchSource::None;

    // Envelope
    int envStage = 0;
    float envLevel = 0.0f, envTime = 0.0f;
    float attack = 0.008f, decay = 0.22f, sustain = 0.68f, release = 0.75f;

    double sampleRate = 44100.0;

    void updateEnvelope();
    double getModulationValue(PatchSource source);
};
