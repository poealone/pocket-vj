#pragma once
#include "fft.h"
#include <SDL2/SDL.h>

#define AUDIO_BUFFER_SIZE 4096
#define AUDIO_SAMPLE_RATE 22050

class AudioInput {
public:
    bool init();
    void shutdown();

    // Call each frame to feed samples to FFT
    void update(FFTAnalyzer& fft);

    // State
    bool isCapturing() const { return m_deviceId != 0; }
    float gain() const { return m_gain; }
    float smoothing() const { return m_smoothing; }
    void setGain(float g) { m_gain = g; }
    void setSmoothing(float s) { m_smoothing = s; }

private:
    SDL_AudioDeviceID m_deviceId = 0;
    float m_gain = 1.0f;
    float m_smoothing = 0.3f;

    // Circular buffer
    float m_buffer[AUDIO_BUFFER_SIZE] = {};
    int   m_writePos = 0;
    int   m_available = 0;

    // SDL audio callback (static)
    static void audioCallback(void* userdata, Uint8* stream, int len);
};
