#pragma once
#include "../visual_node.h"

enum class ShapeType { RECT, CIRCLE, LINE, TRIANGLE };

class ShapesNode : public VisualNode {
public:
    ShapesNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "SHAPE"; }
    const char* description() const override { return "Geometric primitives"; }

    void applyParams() override;
    void syncParams() override;

    ShapeType shape = ShapeType::CIRCLE;
    bool filled = true;
    float rotation = 0.0f;
    float rotSpeed = 1.0f;
    float pulseSpeed = 2.0f;

private:
    float m_pulse = 0.0f;
};
