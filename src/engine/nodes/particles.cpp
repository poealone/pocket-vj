#include "particles.h"
#include <cstdlib>
#include <cmath>

static float randf() { return (float)rand() / RAND_MAX; }
static float randf(float min, float max) { return min + randf() * (max - min); }

ParticlesNode::ParticlesNode() {
    color = Palette::WHITE;
    x = RENDER_W / 2;
    y = RENDER_H / 2;
    w = RENDER_W;
    h = RENDER_H;
}

void ParticlesNode::emit(int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!m_particles[i].alive) {
            Particle& p = m_particles[i];
            p.alive = true;
            p.x = (float)x;
            p.y = (float)y;
            p.vx = randf(-spread, spread);
            p.vy = randf(-spread * 2.0f, -spread * 0.5f);
            p.life = 0.0f;
            p.maxLife = randf(maxLifetime * 0.5f, maxLifetime);
            p.color = color;
            // Slight color variation
            p.color.r = (uint8_t)std::min(255.0f, (float)color.r + randf(-30, 30));
            p.color.g = (uint8_t)std::min(255.0f, (float)color.g + randf(-30, 30));
            p.color.b = (uint8_t)std::min(255.0f, (float)color.b + randf(-30, 30));
            count--;
        }
    }
}

void ParticlesNode::burst(int count) {
    emit(count > 0 ? count : burstCount);
}

void ParticlesNode::update(float dt, float audioLevel) {
    if (!active) return;

    // Auto-emit
    if (m_autoRate > 0.0f) {
        m_timer += dt;
        float interval = 1.0f / m_autoRate;
        while (m_timer >= interval) {
            emit(1);
            m_timer -= interval;
        }
    }

    // Demo mode: periodic bursts
    if (!reactive) {
        static float demoTimer = 0.0f;
        demoTimer += dt;
        if (demoTimer > 0.8f) {
            burst(burstCount);
            demoTimer = 0.0f;
        }
    } else if (audioLevel > 0.7f) {
        // Beat-triggered burst
        burst(burstCount);
    }

    // Update particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = m_particles[i];
        if (!p.alive) continue;

        p.life += dt;
        if (p.life >= p.maxLife) {
            p.alive = false;
            continue;
        }

        p.vy -= gravity * dt * 60.0f;  // Gravity (negative = up)
        p.x += p.vx * dt * 60.0f;
        p.y += p.vy * dt * 60.0f;

        // Fade out
        float alpha = 1.0f - (p.life / p.maxLife);
        p.color.a = (uint8_t)(alpha * 255);
    }
}

void ParticlesNode::render(Renderer& r) {
    if (!active) return;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        const Particle& p = m_particles[i];
        if (!p.alive) continue;

        float alpha = (float)p.color.a / 255.0f;
        Color c = p.color;
        c.r = (uint8_t)(c.r * alpha);
        c.g = (uint8_t)(c.g * alpha);
        c.b = (uint8_t)(c.b * alpha);

        // 2x2 pixel for visibility
        r.pixel((int)p.x, (int)p.y, c);
        r.pixel((int)p.x + 1, (int)p.y, c);
        r.pixel((int)p.x, (int)p.y + 1, c);
        r.pixel((int)p.x + 1, (int)p.y + 1, c);
    }
}

void ParticlesNode::setParam(const std::string& name, float value) {
    if (name == "gravity") gravity = value;
    else if (name == "lifetime") maxLifetime = value;
    else if (name == "spread") spread = value;
    else if (name == "burst") burstCount = (int)value;
    else if (name == "rate") m_autoRate = value;
    else if (name == "intensity") intensity = value;
    else if (name == "color_r") color.r = (uint8_t)value;
    else if (name == "color_g") color.g = (uint8_t)value;
    else if (name == "color_b") color.b = (uint8_t)value;
}
