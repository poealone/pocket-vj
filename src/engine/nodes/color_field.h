#pragma once
#include "../visual_node.h"

enum class FieldType { SOLID, GRADIENT_V, GRADIENT_H, PULSE };

class ColorFieldNode : public VisualNode {
public:
    ColorFieldNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "FIELD"; }
    const char* description() const override { return "Color fill / gradient"; }

    void applyParams() override;
    void syncParams() override;

    FieldType fieldType = FieldType::GRADIENT_V;
    Color color2 = Palette::BLUE;
    float pulseSpeed = 1.0f;

private:
    float m_phase = 0.0f;
};
