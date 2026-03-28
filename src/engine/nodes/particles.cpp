#include "particles.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

static float randf() { return (float)rand() / RAND_MAX; }
static float randf(float mn, float mx) { return mn + randf() * (mx - mn); }

ParticlesNode::ParticlesNode() {
    color = Palette::WHITE;
    x = RENDER_W / 2;
    y = RENDER_H / 2;
    w = RENDER_W;
    h = RENDER_H;

    params.addFloat("gravity", "Gravity", -0.5f, -5.0f, 5.0f, 0.1f);
    params.addFloat("lifetime", "Lifetime", 2.0f, 0.2f, 8.0f, 0.2f);
    params.addFloat("spread", "Spread", 3.0f, 0.5f, 10.0f, 0.5f);
    params.addInt("burst", "Burst Cnt", 8, 1, 32);
    params.addFloat("rate", "Auto Rate", 0.0f, 0.0f, 30.0f, 1.0f);
    params.addFloat("intensity", "Intensity", 1.0f, 0.0f, 2.0f, 0.1f);
    params.addToggle("reactive", "Reactive", false);
    params.addColor("color", "Color", color.r, color.g, color.b);
}

void ParticlesNode::applyParams() {
    gravity = params.get("gravity");
    maxLifetime = params.get("lifetime");
    spread = params.get("spread");
    burstCount = (int)params.get("burst");
    m_autoRate = params.get("rate");
    intensity = params.get("intensity");
    reactive = params.get("reactive") > 0.5f;
    color.r = (uint8_t)params.get("color_r");
    color.g = (uint8_t)params.get("color_g");
    color.b = (uint8_t)params.get("color_b");
}

void ParticlesNode::syncParams() {
    params.set("gravity", gravity);
    params.set("lifetime", maxLifetime);
    params.set("spread", spread);
    params.set("burst", (float)burstCount);
    params.set("rate", m_autoRate);
    params.set("intensity", intensity);
    params.set("reactive", reactive ? 1.0f : 0.0f);
    params.set("color_r", (float)color.r);
    params.set("color_g", (float)color.g);
    params.set("color_b", (float)color.b);
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
            p.color.r = (uint8_t)std::min(255.0f, std::max(0.0f, (float)color.r + randf(-30, 30)));
            p.color.g = (uint8_t)std::min(255.0f, std::max(0.0f, (float)color.g + randf(-30, 30)));
            p.color.b = (uint8_t)std::min(255.0f, std::max(0.0f, (float)color.b + randf(-30, 30)));
            count--;
        }
    }
}

void ParticlesNode::burst(int count) {
    emit(count > 0 ? count : burstCount);
}

void ParticlesNode::update(float dt, float audioLevel) {
    params.animateAll(dt);
    applyParams();
    if (!active) return;

    if (m_autoRate > 0.0f) {
        m_timer += dt;
        float interval = 1.0f / m_autoRate;
        while (m_timer >= interval) {
            emit(1);
            m_timer -= interval;
        }
    }

    if (!reactive) {
        static float demoTimer = 0.0f;
        demoTimer += dt;
        if (demoTimer > 0.8f) {
            burst(burstCount);
            demoTimer = 0.0f;
        }
    } else if (audioLevel > 0.7f) {
        burst(burstCount);
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = m_particles[i];
        if (!p.alive) continue;

        p.life += dt;
        if (p.life >= p.maxLife) {
            p.alive = false;
            continue;
        }

        p.vy -= gravity * dt * 60.0f;
        p.x += p.vx * dt * 60.0f;
        p.y += p.vy * dt * 60.0f;

        float alpha = 1.0f - (p.life / p.maxLife);
        p.color.a = (uint8_t)(alpha * 255);
    }
}

void ParticlesNode::setSpriteFile(const std::string& path) {
    spriteFile = path;
    if (!path.empty()) {
        m_sprite = SpriteCache::instance().load(path);
    } else {
        m_sprite = nullptr;
    }
}

void ParticlesNode::render(Renderer& r) {
    if (!active) return;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        const Particle& p = m_particles[i];
        if (!p.alive) continue;

        float alpha = (float)p.color.a / 255.0f;

        if (m_sprite) {
            // Render custom sprite at particle position
            float lifeRatio = 1.0f - (p.life / p.maxLife);
            float scale = 0.5f + lifeRatio * 0.5f; // Scale down as particle dies
            m_sprite->draw(r.pixels(), RENDER_W, RENDER_H,
                          (int)p.x, (int)p.y, scale,
                          p.color.r, p.color.g, p.color.b, alpha);
        } else {
            // Default 2x2 pixel rendering
            Color c = p.color;
            c.r = (uint8_t)(c.r * alpha);
            c.g = (uint8_t)(c.g * alpha);
            c.b = (uint8_t)(c.b * alpha);

            r.pixel((int)p.x, (int)p.y, c);
            r.pixel((int)p.x + 1, (int)p.y, c);
            r.pixel((int)p.x, (int)p.y + 1, c);
            r.pixel((int)p.x + 1, (int)p.y + 1, c);
        }
    }
}
