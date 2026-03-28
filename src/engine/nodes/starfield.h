#pragma once
#include "../visual_node.h"

struct Star {
    float x, y, z;
};

class StarfieldNode : public VisualNode {
public:
    StarfieldNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "STAR"; }
    const char* description() const override { return "3D star field"; }

    void applyParams() override;
    void syncParams() override;

    int   starCount = 64;
    float speed = 3.0f;
    float depth = 50.0f;

private:
    Star m_stars[MAX_PARTICLES]; // reuse MAX_PARTICLES (128) as limit
    bool m_initialized = false;

    void initStar(Star& s, bool randomZ);
    uint32_t m_seed = 12345;
    float fastRand();
};
