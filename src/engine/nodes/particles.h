#pragma once
#include "../visual_node.h"

struct Particle {
    float x, y;
    float vx, vy;
    float life, maxLife;
    Color color;
    bool alive = false;
};

class ParticlesNode : public VisualNode {
public:
    ParticlesNode();
    void update(float dt, float audioLevel = 0.0f) override;
    void render(Renderer& r) override;
    const char* typeName() const override { return "PTCL"; }
    const char* description() const override { return "Particle emitter"; }

    void applyParams() override;
    void syncParams() override;

    void burst(int count = 0);

    float gravity = -0.5f;
    float maxLifetime = 2.0f;
    float spread = 3.0f;
    int   burstCount = 8;

private:
    Particle m_particles[MAX_PARTICLES];
    float m_timer = 0.0f;
    float m_autoRate = 0.0f;

    void emit(int count);
};
