#pragma once
#include "../visual_node.h"

enum class EnvStage { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

class EnvelopeModulator : public VisualNode {
public:
    EnvelopeModulator();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "ENV"; }
    const char* description() const override { return "ADSR envelope"; }
    NodeCategory category() const override { return NodeCategory::MODULATOR; }

    void applyParams() override;
    void syncParams() override;

    // Output value (0-1)
    float output() const { return m_output; }

    // Trigger the envelope
    void trigger();
    void release();

    float attack = 0.1f;
    float decay = 0.3f;
    float sustain = 0.7f;
    float releaseTime = 0.5f;

private:
    EnvStage m_stage = EnvStage::IDLE;
    float m_output = 0.0f;
    float m_stageTime = 0.0f;
    float m_releaseLevel = 0.0f;
};
