#pragma once
#include "../visual_node.h"

class NoiseNode : public VisualNode {
public:
    NoiseNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "NOISE"; }
    const char* description() const override { return "Procedural noise pattern"; }

    void applyParams() override;
    void syncParams() override;

    float scale = 20.0f;
    float speed = 2.0f;
    int   octaves = 2;
    Color color2 = Palette::BLUE;

private:
    float m_time = 0.0f;

    // Simple hash-based noise
    static float hash(float x, float y);
    static float smoothNoise(float x, float y);
    static float interpolatedNoise(float x, float y);
    float fractalNoise(float x, float y, int oct) const;
};
