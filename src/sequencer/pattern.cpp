#include "pattern.h"
#include <algorithm>

Pattern::Pattern(int length) : m_length(std::min(length, (int)MAX_PATTERN_LEN)) {}

void Pattern::setStep(int step, int nodeIdx, const std::string& param, float value) {
    if (step < 0 || step >= m_length) return;
    m_steps[step].active = true;
    m_steps[step].nodeIndex = nodeIdx;
    m_steps[step].paramName = param;
    m_steps[step].value = value;
}

void Pattern::clearStep(int step) {
    if (step < 0 || step >= m_length) return;
    m_steps[step].active = false;
    m_steps[step].nodeIndex = -1;
    m_steps[step].paramName.clear();
    m_steps[step].value = 0.0f;
}

const PatternStep& Pattern::getStep(int step) const {
    static PatternStep empty;
    if (step < 0 || step >= m_length) return empty;
    return m_steps[step];
}

void Pattern::setLength(int len) {
    m_length = std::max(1, std::min(len, (int)MAX_PATTERN_LEN));
    if (m_currentStep >= m_length) m_currentStep = 0;
}

void Pattern::tick() {
    m_currentStep = (m_currentStep + 1) % m_length;
}

void Pattern::reset() {
    m_currentStep = 0;
    m_accumulator = 0.0f;
}

bool Pattern::hasEvent() const {
    return m_steps[m_currentStep].active;
}

const PatternStep& Pattern::currentEvent() const {
    return m_steps[m_currentStep];
}

void Pattern::update(float dt) {
    if (!m_playing) return;

    // Advance based on BPM (each step = 1/4 beat at given BPM)
    float stepDuration = 60.0f / m_bpm / 4.0f;  // 16th note resolution
    m_accumulator += dt;

    while (m_accumulator >= stepDuration) {
        m_accumulator -= stepDuration;
        tick();
    }
}
