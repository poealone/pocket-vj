#include "waveform.h"
#include <cmath>
#include <algorithm>

WaveformNode::WaveformNode() {
    color = Palette::CYAN;
    y = 80;
    h = 80;
    reactive = true;
}

void WaveformNode::setWaveData(const float* samples, int count) {
    int step = std::max(1, count / WAVE_SAMPLES);
    for (int i = 0; i < WAVE_SAMPLES; i++) {
        int idx = i * step;
        m_wave[i] = (idx < count) ? samples[idx] : 0.0f;
    }
}

void WaveformNode::update(float dt, float audioLevel) {
    m_phase += dt * speed;

    if (!reactive || audioLevel < 0.01f) {
        // Demo mode: generate sine wave
        for (int i = 0; i < WAVE_SAMPLES; i++) {
            float t = (float)i / WAVE_SAMPLES;
            m_wave[i] = sinf(t * 6.28f * 3.0f + m_phase * 4.0f) * intensity;
            // Add harmonics
            m_wave[i] += sinf(t * 6.28f * 7.0f + m_phase * 2.5f) * intensity * 0.3f;
            m_wave[i] += sinf(t * 6.28f * 1.0f + m_phase * 1.0f) * intensity * 0.5f;
        }
    }
}

void WaveformNode::render(Renderer& r) {
    if (!active) return;

    int centerY = y + h / 2;

    for (int i = 0; i < w - 1; i++) {
        int si = (i * WAVE_SAMPLES) / w;
        int si2 = ((i + 1) * WAVE_SAMPLES) / w;
        if (si >= WAVE_SAMPLES || si2 >= WAVE_SAMPLES) break;

        int y1 = centerY + (int)(m_wave[si] * h * 0.5f);
        int y2 = centerY + (int)(m_wave[si2] * h * 0.5f);

        // Draw with thickness
        for (int t = 0; t < thickness; t++) {
            r.line(x + i, y1 + t, x + i + 1, y2 + t, color);
        }
    }
}

void WaveformNode::setParam(const std::string& name, float value) {
    if (name == "thickness") thickness = std::max(1, (int)value);
    else if (name == "speed") speed = value;
    else if (name == "intensity") intensity = value;
    else if (name == "color_r") color.r = (uint8_t)value;
    else if (name == "color_g") color.g = (uint8_t)value;
    else if (name == "color_b") color.b = (uint8_t)value;
}
