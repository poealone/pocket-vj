#include "fft.h"
#include <cmath>
#include <cstdlib>

void FFTAnalyzer::init(int sampleRate, int fftSize) {
    m_binCount = MAX_FFT_BINS;
    for (int i = 0; i < m_binCount; i++) m_bins[i] = 0.0f;
}

void FFTAnalyzer::process(const float* samples, int count) {
    // TODO Phase 3: Real FFT implementation
    // For now this is stubbed — real audio input will feed here
}

void FFTAnalyzer::generateDemo(float dt) {
    m_time += dt;

    // Simulate a beat at ~120 BPM (2 Hz)
    float beatPhase = fmodf(m_time * 2.0f, 1.0f);
    m_beat = (beatPhase < 0.05f);

    // Generate fake frequency data that looks musical
    for (int i = 0; i < m_binCount; i++) {
        float freq = (float)i / m_binCount;

        // Bass-heavy with decay toward highs
        float base = (1.0f - freq * 0.7f);

        // Rhythmic pulse on bass
        float kick = (i < 3) ? (1.0f - beatPhase) * 0.8f : 0.0f;

        // Mid-frequency motion
        float mid = sinf(m_time * 3.0f + i * 0.5f) * 0.3f;

        // High-frequency sparkle
        float hi = (i > m_binCount / 2) ? sinf(m_time * 7.0f + i * 1.3f) * 0.2f : 0.0f;

        m_bins[i] = base * 0.3f + kick + mid * 0.5f + hi;
        if (m_bins[i] < 0.0f) m_bins[i] = 0.0f;
        if (m_bins[i] > 1.0f) m_bins[i] = 1.0f;
    }

    // Derived values
    m_bass = (m_bins[0] + m_bins[1] + m_bins[2]) / 3.0f;
    m_mid = 0.0f;
    m_high = 0.0f;
    for (int i = 3; i < m_binCount / 2; i++) m_mid += m_bins[i];
    for (int i = m_binCount / 2; i < m_binCount; i++) m_high += m_bins[i];
    m_mid /= (m_binCount / 2 - 3);
    m_high /= (m_binCount - m_binCount / 2);
    m_level = m_bass * 0.5f + m_mid * 0.3f + m_high * 0.2f;
}
