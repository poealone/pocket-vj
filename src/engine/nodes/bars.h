#pragma once
#include "../visual_node.h"

class BarsNode : public VisualNode {
public:
    BarsNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "BARS"; }
    const char* description() const override { return "Spectrum analyzer bars"; }

    void applyParams() override;
    void syncParams() override;

    // Set FFT data externally
    void setFFTData(const float* bins, int count);

    int numBars = 16;
    float gap = 1.0f;

private:
    float m_bins[MAX_FFT_BINS] = {};
    float m_smoothBins[MAX_FFT_BINS] = {};
    int   m_binCount = 0;
    float m_fallSpeed = 3.0f;
};
