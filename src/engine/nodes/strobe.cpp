#include "strobe.h"
#include <cmath>

StrobeNode::StrobeNode() {
    color = Palette::WHITE;
    x = 0; y = 0;
    w = RENDER_W; h = RENDER_H;

    params.addFloat("rate", "Rate Hz", 4.0f, 0.1f, 20.0f, 0.1f);
    params.addFloat("duty", "Duty Cycle", 0.5f, 0.0f, 1.0f, 0.05f);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void StrobeNode::applyParams() {
    rate = params.get("rate");
    dutyCycle = params.get("duty");
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void StrobeNode::syncParams() {
    params.set("rate", rate);
    params.set("duty", dutyCycle);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
}

void StrobeNode::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_phase += dt * rate;
    if (m_phase >= 1.0f) m_phase -= 1.0f;
    m_on = (m_phase < dutyCycle);
}

void StrobeNode::render(Renderer& r) {
    if (!active || !m_on) return;
    r.rect(x, y, w, h, color, true);
}
