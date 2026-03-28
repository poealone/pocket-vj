#include "lfo.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LFOModulator::LFOModulator() {
    active = true;

    params.addEnum("shape", "Shape", {"SINE", "TRI", "SAW", "SQUARE"}, 0);
    params.addFloat("rate", "Rate Hz", 1.0f, 0.01f, 20.0f, 0.01f);
    params.addFloat("depth", "Depth", 1.0f, 0.0f, 1.0f, 0.05f);
    params.addFloat("phase", "Phase", 0.0f, 0.0f, 360.0f, 5.0f);
}

void LFOModulator::applyParams() {
    shape = (LFOShape)(int)params.get("shape");
    rate = params.get("rate");
    depth = params.get("depth");
    phase = params.get("phase");
}

void LFOModulator::syncParams() {
    params.set("shape", (float)shape);
    params.set("rate", rate);
    params.set("depth", depth);
    params.set("phase", phase);
}

void LFOModulator::update(float dt, float /*audioLevel*/) {
    params.animateAll(dt);
    applyParams();
    m_phase += dt * rate;
    if (m_phase >= 1.0f) m_phase -= (int)m_phase;

    float p = m_phase + phase / 360.0f;
    if (p >= 1.0f) p -= 1.0f;

    float raw = 0.0f;
    switch (shape) {
        case LFOShape::SINE:
            raw = sinf(p * 2.0f * (float)M_PI) * 0.5f + 0.5f;
            break;
        case LFOShape::TRIANGLE:
            raw = (p < 0.5f) ? (p * 4.0f - 1.0f) : (3.0f - p * 4.0f);
            raw = raw * 0.5f + 0.5f;
            break;
        case LFOShape::SAW:
            raw = p;
            break;
        case LFOShape::SQUARE:
            raw = (p < 0.5f) ? 1.0f : 0.0f;
            break;
    }

    m_output = raw * depth;
}

void LFOModulator::render(Renderer& r) {
    // Modulators don't render visuals — they could show a small indicator
    // but for now they are invisible processing nodes
    (void)r;
}
