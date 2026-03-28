#include "audio_input.h"
#include <cstdio>
#include <cstring>

void AudioInput::audioCallback(void* userdata, Uint8* stream, int len) {
    AudioInput* self = (AudioInput*)userdata;
    int sampleCount = len / (int)sizeof(float);

    const float* samples = (const float*)stream;
    for (int i = 0; i < sampleCount; i++) {
        self->m_buffer[self->m_writePos] = samples[i] * self->m_gain;
        self->m_writePos = (self->m_writePos + 1) % AUDIO_BUFFER_SIZE;
        if (self->m_available < AUDIO_BUFFER_SIZE) {
            self->m_available++;
        }
    }
}

bool AudioInput::init() {
    // Try to open an audio capture device
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = AUDIO_SAMPLE_RATE;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 512;
    want.callback = audioCallback;
    want.userdata = this;

    // Try default capture device
    m_deviceId = SDL_OpenAudioDevice(nullptr, 1, &want, &have, 0);
    if (m_deviceId == 0) {
        SDL_Log("AudioInput: No capture device available (%s) — using demo mode", SDL_GetError());
        return false;
    }

    SDL_Log("AudioInput: Opened capture device (rate=%d, format=0x%x, channels=%d)",
            have.freq, have.format, have.channels);

    // Start capture
    SDL_PauseAudioDevice(m_deviceId, 0);
    return true;
}

void AudioInput::shutdown() {
    if (m_deviceId != 0) {
        SDL_CloseAudioDevice(m_deviceId);
        m_deviceId = 0;
    }
}

void AudioInput::update(FFTAnalyzer& fft) {
    if (m_deviceId == 0 || m_available < FFT_SIZE) {
        // Not enough data — let caller use demo mode
        fft.setHasAudioInput(false);
        return;
    }

    fft.setHasAudioInput(true);

    // Read FFT_SIZE samples from circular buffer
    float samples[FFT_SIZE];
    int readPos = (m_writePos - FFT_SIZE + AUDIO_BUFFER_SIZE) % AUDIO_BUFFER_SIZE;
    for (int i = 0; i < FFT_SIZE; i++) {
        samples[i] = m_buffer[(readPos + i) % AUDIO_BUFFER_SIZE];
    }

    fft.process(samples, FFT_SIZE);
}
