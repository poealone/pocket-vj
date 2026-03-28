#include "config.h"
#include "input.h"
#include "engine/renderer.h"
#include "engine/visual_node.h"
#include "engine/nodes/bars.h"
#include "engine/nodes/waveform.h"
#include "engine/nodes/shapes.h"
#include "engine/nodes/particles.h"
#include "engine/nodes/color_field.h"
#include "engine/audio/fft.h"
#include "sequencer/pattern.h"
#include "ui/tracker_view.h"
#include "ui/preview.h"
#include "ui/menu.h"

#include <vector>
#include <cstdio>

enum class AppMode {
    TRACKER,     // Main tracker view + small preview
    PREVIEW,     // Fullscreen visual preview
    PERFORMANCE  // Live FX mode (Phase 5)
};

int main(int argc, char* argv[]) {
    // --- Init ---
    Renderer renderer;
    if (!renderer.init()) {
        SDL_Log("Failed to initialize renderer");
        return 1;
    }

    Input input;
    FFTAnalyzer fft;
    fft.init();

    // --- Create visual nodes ---
    ColorFieldNode bgField;
    bgField.fieldType = FieldType::GRADIENT_V;

    BarsNode bars;
    bars.x = 0; bars.y = 180; bars.w = RENDER_W; bars.h = 60;
    bars.numBars = 16;
    bars.color = Palette::RED;
    bars.reactive = false;  // Demo mode until audio connected

    WaveformNode wave;
    wave.x = 0; wave.y = 80; wave.w = RENDER_W; wave.h = 80;
    wave.color = Palette::CYAN;
    wave.reactive = false;

    ShapesNode shape;
    shape.x = RENDER_W / 2 - 30; shape.y = 30;
    shape.w = 60; shape.h = 60;
    shape.shape = ShapeType::CIRCLE;
    shape.color = Palette::MAGENTA;

    ParticlesNode particles;
    particles.x = RENDER_W / 2;
    particles.y = 120;
    particles.color = Palette::YELLOW;
    particles.reactive = false;

    // Node list (render order = layer order)
    std::vector<VisualNode*> nodes = {
        &bgField,
        &bars,
        &wave,
        &shape,
        &particles
    };

    int activeNode = 1;  // Currently selected node for editing

    // --- Sequencer ---
    Pattern pattern(16);
    pattern.setBpm(120.0f);

    // Demo pattern
    pattern.setStep(0,  1, "color_r", 255);  // Bars red
    pattern.setStep(4,  1, "color_r", 60);   // Bars blue-ish
    pattern.setStep(4,  1, "color_b", 255);
    pattern.setStep(8,  1, "color_r", 255);  // Back to red
    pattern.setStep(12, 3, "shape", 1);      // Change shape to circle

    // --- UI ---
    TrackerView tracker;
    PreviewPanel preview;
    Menu menu;
    AppMode mode = AppMode::TRACKER;

    // --- Status bar info ---
    char statusBuf[128];

    // --- Main Loop ---
    bool running = true;
    while (running) {
        input.poll();
        if (input.quit()) break;

        float dt = renderer.deltaTime();

        // --- FFT demo data ---
        fft.generateDemo(dt);

        // Feed FFT to bars node
        bars.setFFTData(fft.bins(), fft.binCount());

        // --- Handle mode-specific input ---
        if (menu.isOpen()) {
            menu.update(input);
            if (input.pressed(Button::A)) {
                int sel = menu.selectedIndex();
                if (sel == 5) running = false;  // EXIT
                menu.close();
            }
        } else {
            switch (mode) {
                case AppMode::TRACKER:
                    // Select → open menu
                    if (input.pressed(Button::SELECT)) {
                        menu.open();
                    }
                    // Start → toggle play
                    if (input.pressed(Button::START)) {
                        pattern.togglePlaying();
                    }
                    // L/R → switch active node
                    if (input.pressed(Button::L)) {
                        activeNode = (activeNode - 1 + (int)nodes.size()) % (int)nodes.size();
                    }
                    if (input.pressed(Button::R)) {
                        activeNode = (activeNode + 1) % (int)nodes.size();
                    }
                    // Y → toggle fullscreen preview
                    if (input.pressed(Button::Y)) {
                        mode = AppMode::PREVIEW;
                        preview.fullscreen = true;
                    }
                    // X → toggle node active
                    if (input.pressed(Button::X)) {
                        nodes[activeNode]->active = !nodes[activeNode]->active;
                    }
                    // A → trigger particle burst (if particles selected)
                    if (input.pressed(Button::A)) {
                        if (activeNode == 4) particles.burst();
                    }

                    tracker.update(input, pattern);
                    break;

                case AppMode::PREVIEW:
                    // Any button → back to tracker
                    if (input.pressed(Button::B) || input.pressed(Button::Y)) {
                        mode = AppMode::TRACKER;
                        preview.fullscreen = false;
                    }
                    // Start → toggle play
                    if (input.pressed(Button::START)) {
                        pattern.togglePlaying();
                    }
                    break;

                case AppMode::PERFORMANCE:
                    // TODO Phase 5
                    if (input.pressed(Button::B)) mode = AppMode::TRACKER;
                    break;
            }
        }

        // --- Update ---
        pattern.update(dt);

        // Apply pattern events to nodes
        if (pattern.isPlaying() && pattern.hasEvent()) {
            const PatternStep& ev = pattern.currentEvent();
            if (ev.nodeIndex >= 0 && ev.nodeIndex < (int)nodes.size()) {
                nodes[ev.nodeIndex]->setParam(ev.paramName, ev.value);
            }
        }

        // Update all visual nodes
        for (auto* node : nodes) {
            if (node && node->active) {
                node->update(dt, fft.level());
            }
        }

        // --- Render ---
        renderer.beginFrame();

        if (mode == AppMode::PREVIEW || mode == AppMode::PERFORMANCE) {
            // Fullscreen visual output
            preview.render(renderer, nodes);
        } else {
            // Split view: tracker left, preview right
            // Render visual nodes in preview area
            preview.render(renderer, nodes);

            // Tracker overlay (left side)
            tracker.render(renderer, pattern);

            // Status bar (bottom)
            snprintf(statusBuf, sizeof(statusBuf),
                "BPM:%3.0f  NODE:%s  %s  FRM:%d",
                pattern.bpm(),
                nodes[activeNode]->typeName(),
                pattern.isPlaying() ? "PLAY" : "STOP",
                renderer.frameCount()
            );
            renderer.rect(0, RENDER_H - 10, RENDER_W, 10, {10, 10, 16}, true);
            renderer.text(2, RENDER_H - 9, statusBuf, Palette::UI_FG);

            // Active node indicator
            snprintf(statusBuf, sizeof(statusBuf), "L%d", activeNode);
            renderer.text(RENDER_W - 20, RENDER_H - 9, statusBuf, Palette::RED);
        }

        // Menu on top of everything
        menu.render(renderer);

        renderer.endFrame();
    }

    renderer.shutdown();
    return 0;
}
