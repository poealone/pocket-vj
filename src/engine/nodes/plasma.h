#pragma once
#include "../visual_node.h"

class PlasmaNode : public VisualNode {
public:
    PlasmaNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "PLSMA"; }
    const char* description() const override { return "Plasma sine waves"; }

    void applyParams() override;
    void syncParams() override;

    float speed = 3.0f;
    float scale = 15.0f;
    int   complexity = 3;
    Color color2 = Palette::BLUE;
    Color color3 = Palette::YELLOW;

private:
    float m_time = 0.0f;
};
