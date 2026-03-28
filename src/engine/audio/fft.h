#pragma once
#include "../../config.h"

// FFT size (must be power of 2)
#define FFT_SIZE 256

// Lightweight FFT for audio analysis
class FFTAnalyzer {
public:
    void init(int sampleRate = 22050, int fftSize = FFT_SIZE);
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

    // Is real audio connected?
    bool hasAudioInput() const { return m_hasAudio; }
    void setHasAudioInput(bool v) { m_hasAudio = v; }

    // Demo mode: generate fake reactive data
    void generateDemo(float dt);

private:
    float m_bins[MAX_FFT_BINS] = {};
    int   m_binCount = MAX_FFT_BINS;
    float m_level = 0.0f;
    float m_bass = 0.0f;
    float m_mid = 0.0f;
    float m_high = 0.0f;
    bool  m_beat = false;
    bool  m_hasAudio = false;
    float m_time = 0.0f;
    int   m_sampleRate = 22050;
    int   m_fftSize = FFT_SIZE;

    // Beat detection state
    float m_prevLevel = 0.0f;
    float m_avgLevel = 0.0f;

    // FFT working buffers
    float m_real[FFT_SIZE] = {};
    float m_imag[FFT_SIZE] = {};
    float m_window[FFT_SIZE] = {};  // Hann window

    // Cooley-Tukey in-place radix-2 FFT
    void fftRadix2(float* real, float* imag, int n);
    void computeDerived();
};
