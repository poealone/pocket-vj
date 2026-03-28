#pragma once
#include "../visual_node.h"

class LaserNode : public VisualNode {
public:
    LaserNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "LASER"; }
    const char* description() const override { return "Vector line patterns"; }

    void applyParams() override;
    void syncParams() override;

    int   points = 6;
    float speed = 3.0f;
    float decay = 0.5f;
    int   thickness = 2;

private:
    float m_angle = 0.0f;
    float m_pulse = 0.0f;
};
