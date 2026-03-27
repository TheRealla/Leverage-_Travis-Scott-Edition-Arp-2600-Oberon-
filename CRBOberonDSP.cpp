// In CRBOberonDSP.h private section:
ARP2600Rectifier rectifier;
bool rectifierEnabled = false;   // Linked to red DRIVE button or separate switch

// In CRBOberonDSP.cpp
void CRBOberonDSP::prepare(double sampleRate)
{
    vco1.prepare(sampleRate);
    vco2.prepare(sampleRate);
    vco3.prepare(sampleRate);
    filter.prepare(sampleRate);
    rectifier.prepare(sampleRate);
    this->sampleRate = sampleRate;
}

float CRBOberonDSP::process()
{
    double o1 = vco1.process();
    double o2 = vco2.process();
    double amMod = (vco3.process() + 1.0) * 0.5;
    double oscMix = (o1 + o2 * amMod) * 0.5;

    double filtered = filter.process(oscMix);

    // ARP 2600 Rectifier on left side
    double rectCV = rectifier.process(oscMix);   // or external input if you add sidechain

    // Optional: use rectCV to modulate Oberon filter cutoff (classic ARP trick)
    if (rectifierEnabled) {
        filter.setCutoff(800.0 + rectCV * 8000.0);  // Dynamic modulation example
    }

    updateEnvelope();
    return limiter.process(filtered * envLevel, preamp);
}

// New setter
void CRBOberonDSP::setRectifierEnabled(bool enabled)
{
    rectifierEnabled = enabled;
}
