#pragma once
#include "../../config.h"

// Lightweight FFT for audio analysis
// Phase 3 will implement real FFT — this is the interface + demo data
class FFTAnalyzer {
public:
    void init(int sampleRate = 22050, int fftSize = 256);
    void process(const float* samples, int count);

    // Get frequency bin magnitudes (0.0 - 1.0)
    const float* bins() const { return m_bins; }
    int binCount() const { return m_binCount; }

    // Derived values
    float level() const { return m_level; }     // Overall amplitude
    float bass() const { return m_bass; }       // Low freq energy
    float mid() const { return m_mid; }         // Mid freq energy
    float high() const { return m_high; }       // High freq energy
    bool beat() const { return m_beat; }        // Beat detected this frame

    // Demo mode: generate fake reactive data
    void generateDemo(float dt);

private:
    float m_bins[MAX_FFT_BINS] = {};
    int   m_binCount = 16;
    float m_level = 0.0f;
    float m_bass = 0.0f;
    float m_mid = 0.0f;
    float m_high = 0.0f;
    bool  m_beat = false;
    float m_time = 0.0f;
};
