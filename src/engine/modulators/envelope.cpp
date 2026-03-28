#include "envelope.h"

EnvelopeModulator::EnvelopeModulator() {
    active = true;

    params.addFloat("attack", "Attack", 0.1f, 0.0f, 5.0f, 0.01f);
    params.addFloat("decay", "Decay", 0.3f, 0.0f, 5.0f, 0.01f);
    params.addFloat("sustain", "Sustain", 0.7f, 0.0f, 1.0f, 0.05f);
    params.addFloat("release", "Release", 0.5f, 0.0f, 5.0f, 0.01f);
}

void EnvelopeModulator::applyParams() {
    attack = params.get("attack");
    decay = params.get("decay");
    sustain = params.get("sustain");
    releaseTime = params.get("release");
}

void EnvelopeModulator::syncParams() {
    params.set("attack", attack);
    params.set("decay", decay);
    params.set("sustain", sustain);
    params.set("release", releaseTime);
}

void EnvelopeModulator::trigger() {
    m_stage = EnvStage::ATTACK;
    m_stageTime = 0.0f;
}

void EnvelopeModulator::release() {
    if (m_stage != EnvStage::IDLE) {
        m_stage = EnvStage::RELEASE;
        m_releaseLevel = m_output;
        m_stageTime = 0.0f;
    }
}

void EnvelopeModulator::update(float dt, float audioLevel) {
    // Auto-trigger on beat if reactive
    if (reactive && audioLevel > 0.5f && m_stage == EnvStage::IDLE) {
        trigger();
    }

    m_stageTime += dt;

    switch (m_stage) {
        case EnvStage::IDLE:
            m_output = 0.0f;
            break;

        case EnvStage::ATTACK:
            if (attack <= 0.001f) {
                m_output = 1.0f;
                m_stage = EnvStage::DECAY;
                m_stageTime = 0.0f;
            } else {
                m_output = m_stageTime / attack;
                if (m_output >= 1.0f) {
                    m_output = 1.0f;
                    m_stage = EnvStage::DECAY;
                    m_stageTime = 0.0f;
                }
            }
            break;

        case EnvStage::DECAY:
            if (decay <= 0.001f) {
                m_output = sustain;
                m_stage = EnvStage::SUSTAIN;
                m_stageTime = 0.0f;
            } else {
                m_output = 1.0f - (1.0f - sustain) * (m_stageTime / decay);
                if (m_stageTime >= decay) {
                    m_output = sustain;
                    m_stage = EnvStage::SUSTAIN;
                    m_stageTime = 0.0f;
                }
            }
            break;

        case EnvStage::SUSTAIN:
            m_output = sustain;
            // Auto-release after 2 seconds if reactive
            if (reactive && m_stageTime > 2.0f) {
                release();
            }
            break;

        case EnvStage::RELEASE:
            if (releaseTime <= 0.001f) {
                m_output = 0.0f;
                m_stage = EnvStage::IDLE;
            } else {
                m_output = m_releaseLevel * (1.0f - m_stageTime / releaseTime);
                if (m_output <= 0.0f) {
                    m_output = 0.0f;
                    m_stage = EnvStage::IDLE;
                }
            }
            break;
    }

    if (m_output < 0.0f) m_output = 0.0f;
    if (m_output > 1.0f) m_output = 1.0f;
}

void EnvelopeModulator::render(Renderer& r) {
    (void)r; // Modulators don't render visuals
}
