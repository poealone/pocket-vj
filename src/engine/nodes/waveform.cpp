#include "waveform.h"
#include <cmath>
#include <algorithm>

WaveformNode::WaveformNode() {
    color = Palette::CYAN;
    y = 80;
    h = 80;
    reactive = true;
    thickness = 2;
    speed = 1.0f;
    intensity = 1.0f;

    params.addInt("thickness", "Thickness", 2, 1, 6);
    params.addFloat("speed", "Speed", 1.0f, 0.1f, 5.0f, 0.1f);
    params.addFloat("intensity", "Intensity", 1.0f, 0.0f, 2.0f, 0.1f);
    params.addToggle("reactive", "Reactive", true);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void WaveformNode::applyParams() {
    thickness = std::max(1, (int)params.get("thickness"));
    speed = params.get("speed");
    intensity = params.get("intensity");
    reactive = params.get("reactive") > 0.5f;
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void WaveformNode::syncParams() {
    params.set("thickness", (float)thickness);
    params.set("speed", speed);
    params.set("intensity", intensity);
    params.set("reactive", reactive ? 1.0f : 0.0f);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
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
        for (int i = 0; i < WAVE_SAMPLES; i++) {
            float t = (float)i / WAVE_SAMPLES;
            m_wave[i] = sinf(t * 6.28f * 3.0f + m_phase * 4.0f) * intensity;
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

        for (int t = 0; t < thickness; t++) {
            r.line(x + i, y1 + t, x + i + 1, y2 + t, color);
        }
    }
}
