#include "bars.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

BarsNode::BarsNode() {
    color = Palette::RED;
    y = 180;
    h = 60;
    reactive = true;
    numBars = 16;
    m_fallSpeed = 3.0f;
    intensity = 1.0f;
    gap = 1.0f;

    // Register parameters
    params.addInt("bars", "Bar Count", 16, 1, MAX_FFT_BINS);
    params.addFloat("intensity", "Intensity", 1.0f, 0.0f, 2.0f, 0.1f);
    params.addFloat("fall", "Fall Speed", 3.0f, 0.5f, 10.0f, 0.5f);
    params.addFloat("gap", "Gap", 1.0f, 0.0f, 8.0f, 1.0f);
    params.addToggle("reactive", "Reactive", true);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void BarsNode::applyParams() {
    numBars = std::max(1, std::min((int)params.get("bars"), (int)MAX_FFT_BINS));
    intensity = params.get("intensity");
    m_fallSpeed = params.get("fall");
    gap = params.get("gap");
    reactive = params.get("reactive") > 0.5f;
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void BarsNode::syncParams() {
    params.set("bars", (float)numBars);
    params.set("intensity", intensity);
    params.set("fall", m_fallSpeed);
    params.set("gap", gap);
    params.set("reactive", reactive ? 1.0f : 0.0f);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

void BarsNode::setFFTData(const float* bins, int count) {
    m_binCount = std::min(count, (int)MAX_FFT_BINS);
    for (int i = 0; i < m_binCount; i++) {
        m_bins[i] = bins[i];
    }
}

void BarsNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    for (int i = 0; i < numBars; i++) {
        float target = (i < m_binCount) ? m_bins[i] : 0.0f;

        if (reactive) {
            target *= intensity;
        } else {
            float phase = (SDL_GetTicks() / 1000.0f) * 2.0f + i * 0.3f;
            target = (sinf(phase) * 0.5f + 0.5f) * intensity;
        }

        if (target > m_smoothBins[i]) {
            m_smoothBins[i] = target;
        } else {
            m_smoothBins[i] -= m_fallSpeed * dt;
            if (m_smoothBins[i] < 0.0f) m_smoothBins[i] = 0.0f;
        }
    }
}

void BarsNode::render(Renderer& r) {
    if (!active) return;

    float barW = (float)(w - (numBars - 1) * gap) / numBars;
    if (barW < 1.0f) barW = 1.0f;

    for (int i = 0; i < numBars; i++) {
        float val = m_smoothBins[i];
        int barH = (int)(val * h);
        if (barH < 1) barH = 1;

        int bx = x + (int)(i * (barW + gap));
        int by = y + h - barH;

        Color c = color;
        c.r = (uint8_t)std::min(255, (int)(c.r * (0.5f + val * 0.5f)));
        c.g = (uint8_t)std::min(255, (int)(c.g * (0.5f + val * 0.5f)));
        c.b = (uint8_t)std::min(255, (int)(c.b * (0.5f + val * 0.5f)));

        r.bar(bx, by, (int)barW, barH, c);
    }
}
