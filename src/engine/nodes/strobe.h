#pragma once
#include "../visual_node.h"

class StrobeNode : public VisualNode {
public:
    StrobeNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "STROB"; }
    const char* description() const override { return "Flash/pulse strobe"; }

    void applyParams() override;
    void syncParams() override;

    float rate = 4.0f;
    float dutyCycle = 0.5f;

private:
    float m_phase = 0.0f;
    bool  m_on = false;
};
