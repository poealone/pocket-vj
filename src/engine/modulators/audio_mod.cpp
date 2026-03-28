#include "audio_mod.h"

AudioModulator::AudioModulator() {
    active = true;

    params.addEnum("source", "Source", {"BASS", "MID", "HIGH", "LEVEL", "BEAT"}, 0);
    params.addFloat("gain", "Gain", 1.0f, 0.0f, 5.0f, 0.1f);
    params.addFloat("smooth", "Smoothing", 0.3f, 0.0f, 1.0f, 0.05f);
}

void AudioModulator::applyParams() {
    source = (AudioSource)(int)params.get("source");
    gain = params.get("gain");
    smoothing = params.get("smooth");
}

void AudioModulator::syncParams() {
    params.set("source", (float)source);
    params.set("gain", gain);
    params.set("smooth", smoothing);
}

void AudioModulator::feed(const FFTAnalyzer& fft) {
    float raw = 0.0f;
    switch (source) {
        case AudioSource::BASS:  raw = fft.bass();  break;
        case AudioSource::MID:   raw = fft.mid();   break;
        case AudioSource::HIGH:  raw = fft.high();  break;
        case AudioSource::LEVEL: raw = fft.level(); break;
        case AudioSource::BEAT:  raw = fft.beat() ? 1.0f : 0.0f; break;
    }

    raw *= gain;
    if (raw > 1.0f) raw = 1.0f;

    // Smooth
    m_smoothed = m_smoothed * smoothing + raw * (1.0f - smoothing);
    m_output = m_smoothed;
}

void AudioModulator::update(float /*dt*/, float /*audioLevel*/) {
    // feed() is called separately by main loop
}

void AudioModulator::render(Renderer& r) {
    (void)r; // Modulators don't render visuals
}
