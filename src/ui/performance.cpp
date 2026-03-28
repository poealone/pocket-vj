#include "performance.h"
#include <cstdio>
#include <cstring>

PerformanceMode::PerformanceMode() {}

void PerformanceMode::init(LayerManager* layers, SceneManager* scenes,
                           BpmTap* bpm, Pattern* pattern, Recorder* recorder) {
    m_layers = layers;
    m_scenes = scenes;
    m_bpm = bpm;
    m_pattern = pattern;
    m_recorder = recorder;
    m_selectedFX = 0;
    m_crossfadeMode = false;
    m_crossfading = false;
    overlayVisible = true;

    autoAssignFX(*layers);
}

void PerformanceMode::autoAssignFX(LayerManager& layers) {
    // Auto-assign FX nodes to the 3 slots
    int slot = 0;
    auto allNodes = layers.allNodes();

    for (int i = 0; i < (int)allNodes.size() && slot < 3; i++) {
        VisualNode* node = allNodes[i];
        if (node && node->category() == NodeCategory::FX) {
            m_fxSlots[slot] = i;
            m_fxNames[slot] = node->typeName();
            // Try to find an "intensity" or similar param
            m_fxValues[slot] = node->intensity;
            slot++;
        }
    }

    // Fill remaining slots with "---"
    for (int i = slot; i < 3; i++) {
        m_fxSlots[i] = -1;
        m_fxNames[i] = "---";
        m_fxValues[i] = 0.0f;
    }
}

bool PerformanceMode::update(Input& input) {
    if (!m_layers || !m_scenes) return false;

    float dt = 1.0f / TARGET_FPS;

    // Select: exit performance mode
    if (input.pressed(Button::SELECT)) {
        return true;
    }

    // Y: toggle overlay
    if (input.pressed(Button::Y)) {
        overlayVisible = !overlayVisible;
    }

    // Up/Down: select FX slot
    if (input.pressed(Button::UP)) {
        m_selectedFX = (m_selectedFX - 1 + 3) % 3;
    }
    if (input.pressed(Button::DOWN)) {
        m_selectedFX = (m_selectedFX + 1) % 3;
    }

    // Left/Right: adjust FX intensity
    if (input.held(Button::LEFT) || input.held(Button::RIGHT)) {
        int idx = m_fxSlots[m_selectedFX];
        if (idx >= 0) {
            auto allNodes = m_layers->allNodes();
            if (idx < (int)allNodes.size()) {
                float delta = input.held(Button::RIGHT) ? 0.02f : -0.02f;
                float val = m_fxValues[m_selectedFX] + delta;
                if (val < 0.0f) val = 0.0f;
                if (val > 1.0f) val = 1.0f;
                m_fxValues[m_selectedFX] = val;

                // Apply to node's intensity param
                allNodes[idx]->intensity = val;
                allNodes[idx]->setParam("intensity", val);
            }
        }
    }

    // A: trigger particle burst
    if (input.pressed(Button::A)) {
        triggerBurst();
    }

    // X: next scene (instant)
    if (input.pressed(Button::X)) {
        int cur = m_scenes->currentScene();
        // Find next active scene
        for (int i = 1; i <= MAX_SCENES; i++) {
            int next = (cur + i) % MAX_SCENES;
            if (m_scenes->hasScene(next)) {
                if (m_crossfadeMode) {
                    // Start crossfade
                    m_crossfading = true;
                    m_crossfadeFrom = cur;
                    m_crossfadeTo = next;
                    m_crossfadeProgress = 0.0f;
                } else {
                    m_scenes->loadScene(next, *m_layers);
                    autoAssignFX(*m_layers);
                }
                break;
            }
        }
    }

    // B: toggle instant vs crossfade mode
    if (input.pressed(Button::B)) {
        m_crossfadeMode = !m_crossfadeMode;
    }

    // L/R: crossfade to prev/next scene
    if (input.pressed(Button::L) || input.pressed(Button::R)) {
        int cur = m_scenes->currentScene();
        int dir = input.pressed(Button::R) ? 1 : -1;
        for (int i = 1; i <= MAX_SCENES; i++) {
            int next = (cur + dir * i + MAX_SCENES) % MAX_SCENES;
            if (m_scenes->hasScene(next)) {
                m_crossfading = true;
                m_crossfadeFrom = cur;
                m_crossfadeTo = next;
                m_crossfadeProgress = 0.0f;
                break;
            }
        }
    }

    // Start: BPM tap
    if (input.pressed(Button::START)) {
        m_bpm->tap();
        if (m_bpm->isValid()) {
            float newBpm = m_bpm->getBpm();
            m_pattern->setBpm(newBpm);
        }
    }

    // Update crossfade
    if (m_crossfading) {
        m_crossfadeProgress += CROSSFADE_SPEED * dt;
        if (m_crossfadeProgress >= 1.0f) {
            m_crossfadeProgress = 1.0f;
            m_crossfading = false;
            m_scenes->loadScene(m_crossfadeTo, *m_layers);
            autoAssignFX(*m_layers);
        } else {
            m_scenes->crossfade(m_crossfadeFrom, m_crossfadeTo,
                               m_crossfadeProgress, *m_layers);
        }
    }

    return false;
}

void PerformanceMode::renderOverlay(Renderer& r) {
    if (!overlayVisible) return;

    // Status bar at top
    char buf[128];
    float bpm = m_pattern ? m_pattern->bpm() : 120.0f;
    int sceneNum = m_scenes ? m_scenes->currentScene() + 1 : 1;

    // Semi-transparent top bar
    r.rect(0, 0, RENDER_W, 11, {0, 0, 0, 180}, true);
    snprintf(buf, sizeof(buf), "LIVE ---- %.0f BPM ---- SCENE %d/%d",
             bpm, sceneNum, MAX_SCENES);
    r.text(4, 2, buf, Palette::RED);

    // Recording indicator
    if (m_recorder && m_recorder->isRecording()) {
        r.circle(RENDER_W - 8, 5, 3, {255, 0, 0}, true);
    }

    // FX slots area (bottom portion)
    int fxY = RENDER_H - 56;
    r.rect(0, fxY, RENDER_W, 56, {0, 0, 0, 140}, true);

    for (int i = 0; i < 3; i++) {
        int iy = fxY + 4 + i * 12;
        bool sel = (i == m_selectedFX);

        // FX label
        snprintf(buf, sizeof(buf), "FX%d: %-4s", i + 1, m_fxNames[i].c_str());
        Color labelColor = sel ? Palette::RED : Palette::UI_FG;
        r.text(4, iy, buf, labelColor);

        // Slider
        int sliderX = 72;
        int sliderW = 60;
        drawSlider(r, sliderX, iy + 1, sliderW, m_fxValues[i],
                   sel ? Palette::RED : Palette::CYAN);

        // Percentage
        snprintf(buf, sizeof(buf), "%3d%%", (int)(m_fxValues[i] * 100));
        r.text(sliderX + sliderW + 4, iy, buf, labelColor);

        // Selection indicator
        if (sel) {
            r.text(sliderX + sliderW + 32, iy, "<->", Palette::YELLOW);
        }
    }

    // Help bar at very bottom
    r.rect(0, RENDER_H - 10, RENDER_W, 10, {0, 0, 0, 200}, true);

    const char* modeStr = m_crossfadeMode ? "XFADE" : "INST";
    snprintf(buf, sizeof(buf), "^v:FX <>:AMT A:BURST X:SCN B:%s", modeStr);
    r.text(4, RENDER_H - 9, buf, Palette::GRID);
}

void PerformanceMode::drawSlider(Renderer& r, int x, int y, int w, float value, Color c) {
    // Background track
    r.rect(x, y, w, 6, {30, 30, 40}, true);
    // Filled portion
    int fillW = (int)(w * value);
    if (fillW > 0) {
        r.rect(x, y, fillW, 6, c, true);
    }
    // Border
    r.rect(x, y, w, 6, Palette::GRID, false);
}

void PerformanceMode::triggerBurst() {
    if (!m_layers) return;
    auto allNodes = m_layers->allNodes();
    for (auto* node : allNodes) {
        ParticlesNode* particles = dynamic_cast<ParticlesNode*>(node);
        if (particles && particles->active) {
            particles->burst();
        }
    }
}
