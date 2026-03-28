#include "bars.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

BarsNode::BarsNode() {
    color = Palette::RED;
    y = 180;
    h = 60;
    reactive = true;
}

void BarsNode::setFFTData(const float* bins, int count) {
    m_binCount = std::min(count, (int)MAX_FFT_BINS);
    for (int i = 0; i < m_binCount; i++) {
        m_bins[i] = bins[i];
    }
}

void BarsNode::update(float dt, float audioLevel) {
    // Smooth fall-off for bars
    for (int i = 0; i < numBars; i++) {
        float target = (i < m_binCount) ? m_bins[i] : 0.0f;

        if (reactive) {
            // Audio-reactive: use FFT data
            target *= intensity;
        } else {
            // Demo mode: generate fake data
            float phase = (SDL_GetTicks() / 1000.0f) * 2.0f + i * 0.3f;
            target = (sinf(phase) * 0.5f + 0.5f) * intensity;
        }

        // Smooth rise, slower fall
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

        // Color gradient: base color → brighter at peak
        Color c = color;
        c.r = std::min(255, (int)(c.r * (0.5f + val * 0.5f)));
        c.g = std::min(255, (int)(c.g * (0.5f + val * 0.5f)));
        c.b = std::min(255, (int)(c.b * (0.5f + val * 0.5f)));

        r.bar(bx, by, (int)barW, barH, c);
    }
}

void BarsNode::setParam(const std::string& name, float value) {
    if (name == "bars") numBars = std::max(1, std::min((int)value, (int)MAX_FFT_BINS));
    else if (name == "intensity") intensity = value;
    else if (name == "fall") m_fallSpeed = value;
    else if (name == "color_r") color.r = (uint8_t)value;
    else if (name == "color_g") color.g = (uint8_t)value;
    else if (name == "color_b") color.b = (uint8_t)value;
}

float BarsNode::getParam(const std::string& name) const {
    if (name == "bars") return (float)numBars;
    if (name == "intensity") return intensity;
    if (name == "fall") return m_fallSpeed;
    return 0.0f;
}
