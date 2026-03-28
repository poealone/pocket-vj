#pragma once
#include "../engine/renderer.h"
#include "../engine/layer.h"
#include "../engine/scene.h"
#include "../engine/bpm.h"
#include "../engine/recorder.h"
#include "../engine/visual_node.h"
#include "../engine/nodes/particles.h"
#include "../input.h"
#include "../sequencer/pattern.h"
#include <string>

// Performance Mode: fullscreen visuals with live FX overlay
class PerformanceMode {
public:
    PerformanceMode();

    // Initialize with references to engine systems
    void init(LayerManager* layers, SceneManager* scenes,
              BpmTap* bpm, Pattern* pattern, Recorder* recorder);

    // Returns true if performance mode should exit
    bool update(Input& input);

    // Render overlay on top of visual output
    void renderOverlay(Renderer& r);

    // Assign FX nodes to the 3 FX slots
    void autoAssignFX(LayerManager& layers);

    bool overlayVisible = true;

private:
    LayerManager* m_layers = nullptr;
    SceneManager* m_scenes = nullptr;
    BpmTap*       m_bpm = nullptr;
    Pattern*      m_pattern = nullptr;
    Recorder*     m_recorder = nullptr;

    // FX slots — indices into allNodes()
    int  m_fxSlots[3] = {-1, -1, -1};
    std::string m_fxNames[3] = {"---", "---", "---"};
    float m_fxValues[3] = {0.0f, 0.0f, 0.0f};

    int  m_selectedFX = 0;        // Currently selected FX slot (0-2)
    bool m_crossfadeMode = false; // B toggles instant vs crossfade

    // Crossfade state
    bool  m_crossfading = false;
    int   m_crossfadeFrom = 0;
    int   m_crossfadeTo = 0;
    float m_crossfadeProgress = 0.0f;
    static constexpr float CROSSFADE_SPEED = 1.5f;  // Per second

    // Draw a small slider bar
    void drawSlider(Renderer& r, int x, int y, int w, float value, Color c);

    // Trigger burst on all particle nodes
    void triggerBurst();
};
