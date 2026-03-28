#include "fft.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void FFTAnalyzer::init(int sampleRate, int fftSize) {
    m_sampleRate = sampleRate;
    m_fftSize = fftSize;
    if (m_fftSize > FFT_SIZE) m_fftSize = FFT_SIZE;
    m_binCount = MAX_FFT_BINS;

    // Pre-compute Hann window
    for (int i = 0; i < m_fftSize; i++) {
        m_window[i] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (m_fftSize - 1)));
    }

    for (int i = 0; i < m_binCount; i++) m_bins[i] = 0.0f;
}

// Bit-reversal permutation
static int bitReverse(int x, int log2n) {
    int result = 0;
    for (int i = 0; i < log2n; i++) {
        result = (result << 1) | (x & 1);
        x >>= 1;
    }
    return result;
}

// Cooley-Tukey radix-2 in-place FFT
void FFTAnalyzer::fftRadix2(float* real, float* imag, int n) {
    // Compute log2(n)
    int log2n = 0;
    for (int tmp = n; tmp > 1; tmp >>= 1) log2n++;

    // Bit-reversal permutation
    for (int i = 0; i < n; i++) {
        int j = bitReverse(i, log2n);
        if (j > i) {
            float tr = real[i]; real[i] = real[j]; real[j] = tr;
            float ti = imag[i]; imag[i] = imag[j]; imag[j] = ti;
        }
    }

    // Butterfly operations
    for (int size = 2; size <= n; size *= 2) {
        int halfSize = size / 2;
        float angle = -2.0f * (float)M_PI / size;

        for (int i = 0; i < n; i += size) {
            for (int j = 0; j < halfSize; j++) {
                float wr = cosf(angle * j);
                float wi = sinf(angle * j);

                int even = i + j;
                int odd = i + j + halfSize;

                float tr = real[odd] * wr - imag[odd] * wi;
                float ti = real[odd] * wi + imag[odd] * wr;

                real[odd] = real[even] - tr;
                imag[odd] = imag[even] - ti;
                real[even] += tr;
                imag[even] += ti;
            }
        }
    }
}

void FFTAnalyzer::process(const float* samples, int count) {
    // Copy samples into working buffer with windowing
    int n = (count < m_fftSize) ? count : m_fftSize;
    for (int i = 0; i < n; i++) {
        m_real[i] = samples[i] * m_window[i];
        m_imag[i] = 0.0f;
    }
    // Zero-pad if needed
    for (int i = n; i < m_fftSize; i++) {
        m_real[i] = 0.0f;
        m_imag[i] = 0.0f;
    }

    // Run FFT
    fftRadix2(m_real, m_imag, m_fftSize);

    // Compute magnitude spectrum and bin into MAX_FFT_BINS bands
    int specBins = m_fftSize / 2; // only first half is useful
    int binsPerBand = specBins / m_binCount;
    if (binsPerBand < 1) binsPerBand = 1;

    float maxMag = 0.0001f; // avoid division by zero
    for (int b = 0; b < m_binCount; b++) {
        float sum = 0.0f;
        int start = b * binsPerBand;
        int end = start + binsPerBand;
        if (end > specBins) end = specBins;

        for (int i = start; i < end; i++) {
            float mag = sqrtf(m_real[i] * m_real[i] + m_imag[i] * m_imag[i]);
            sum += mag;
        }
        sum /= binsPerBand;
        m_bins[b] = sum;
        if (sum > maxMag) maxMag = sum;
    }

    // Normalize bins to 0-1 range
    float invMax = 1.0f / maxMag;
    for (int b = 0; b < m_binCount; b++) {
        m_bins[b] *= invMax;
        if (m_bins[b] > 1.0f) m_bins[b] = 1.0f;
    }

    computeDerived();
}

void FFTAnalyzer::computeDerived() {
    // Derived values from bins
    m_bass = 0.0f;
    m_mid = 0.0f;
    m_high = 0.0f;

    int bassEnd = m_binCount / 4;
    int midEnd = m_binCount * 3 / 4;

    for (int i = 0; i < bassEnd; i++) m_bass += m_bins[i];
    m_bass /= bassEnd;

    for (int i = bassEnd; i < midEnd; i++) m_mid += m_bins[i];
    m_mid /= (midEnd - bassEnd);

    for (int i = midEnd; i < m_binCount; i++) m_high += m_bins[i];
    m_high /= (m_binCount - midEnd);

    m_level = m_bass * 0.5f + m_mid * 0.3f + m_high * 0.2f;

    // Simple beat detection: level spike above running average
    m_avgLevel = m_avgLevel * 0.95f + m_level * 0.05f;
    m_beat = (m_level > m_avgLevel * 1.5f && m_level > 0.15f && m_level > m_prevLevel);
    m_prevLevel = m_level;
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
