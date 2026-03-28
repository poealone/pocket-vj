#pragma once
#include "../visual_node.h"

enum class LFOShape { SINE, TRIANGLE, SAW, SQUARE };

class LFOModulator : public VisualNode {
public:
    LFOModulator();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "LFO"; }
    const char* description() const override { return "LFO modulator"; }
    NodeCategory category() const override { return NodeCategory::MODULATOR; }

    void applyParams() override;
    void syncParams() override;

    // Output value (0-1)
    float output() const { return m_output; }

    LFOShape shape = LFOShape::SINE;
    float rate = 1.0f;
    float depth = 1.0f;
    float phase = 0.0f;

private:
    float m_phase = 0.0f;
    float m_output = 0.0f;
};
