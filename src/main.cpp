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
#include "ui/node_editor.h"
#include "ui/node_browser.h"
#include "preset.h"

#include <vector>
#include <cstdio>
#include <sys/stat.h>

enum class AppMode {
    TRACKER,       // Main tracker view + small preview
    PREVIEW,       // Fullscreen visual preview
    NODE_EDITOR,   // Parameter editing for selected node
    NODE_BROWSER,  // Add new node browser
    PRESET_BROWSER,// Save/load presets
    PERFORMANCE    // Live FX mode (Phase 5)
};

// Presets directory path
static const char* PRESETS_DIR = "presets";

static void ensurePresetsDir() {
    mkdir(PRESETS_DIR, 0755);
}

// Create a new node by type ID (from node browser)
static VisualNode* createNode(int typeId) {
    switch (typeId) {
        case 0: return new BarsNode();
        case 1: return new WaveformNode();
        case 2: return new ShapesNode();
        case 3: return new ParticlesNode();
        case 4: return new ColorFieldNode();
        default: return nullptr;
    }
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // --- Init ---
    Renderer renderer;
    if (!renderer.init()) {
        SDL_Log("Failed to initialize renderer");
        return 1;
    }

    Input input;
    input.init();
    FFTAnalyzer fft;
    fft.init();

    ensurePresetsDir();

    // --- Create visual nodes ---
    ColorFieldNode bgField;
    bgField.fieldType = FieldType::GRADIENT_V;

    BarsNode bars;
    bars.x = 0; bars.y = 180; bars.w = RENDER_W; bars.h = 60;
    bars.numBars = 16;
    bars.color = Palette::RED;
    bars.reactive = false;

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

    // Sync initial state to params
    bgField.syncParams();
    bars.syncParams();
    wave.syncParams();
    shape.syncParams();
    particles.syncParams();

    // Node list (render order = layer order)
    // We use a vector of owned pointers for dynamic add/remove
    std::vector<VisualNode*> nodes;
    nodes.push_back(&bgField);
    nodes.push_back(&bars);
    nodes.push_back(&wave);
    nodes.push_back(&shape);
    nodes.push_back(&particles);

    // Track which nodes are heap-allocated (for cleanup)
    // The initial 5 are stack-allocated, dynamically added ones are heap
    int staticNodeCount = (int)nodes.size();

    int activeNode = 1;

    // --- Sequencer ---
    Pattern pattern(16);
    pattern.setBpm(120.0f);

    pattern.setStep(0,  1, "color_r", 255);
    pattern.setStep(4,  1, "color_r", 60);
    pattern.setStep(4,  1, "color_b", 255);
    pattern.setStep(8,  1, "color_r", 255);
    pattern.setStep(12, 3, "shape", 1);

    // --- UI ---
    TrackerView tracker;
    PreviewPanel preview;
    Menu menu;
    NodeEditor nodeEditor;
    NodeBrowser nodeBrowser;
    PresetBrowser presetBrowser;
    AppMode mode = AppMode::TRACKER;

    char statusBuf[128];

    // --- Main Loop ---
    bool running = true;
    while (running) {
        input.poll();
        if (input.quit() || renderer.shouldQuit()) break;

        float dt = renderer.deltaTime();

        // --- FFT demo data ---
        fft.generateDemo(dt);
        bars.setFFTData(fft.bins(), fft.binCount());

        // --- Handle mode-specific input ---
        if (menu.isOpen()) {
            menu.update(input);
            if (input.pressed(Button::A)) {
                int sel = menu.selectedIndex();
                menu.close();
                switch (sel) {
                    case 0: // SAVE PRESET
                        ensurePresetsDir();
                        presetBrowser.open(PRESETS_DIR, true);
                        mode = AppMode::PRESET_BROWSER;
                        break;
                    case 1: // LOAD PRESET
                        ensurePresetsDir();
                        presetBrowser.open(PRESETS_DIR, false);
                        mode = AppMode::PRESET_BROWSER;
                        break;
                    case 5: // EXIT
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        } else {
            switch (mode) {
                case AppMode::TRACKER:
                    if (input.pressed(Button::SELECT)) {
                        menu.open();
                    }
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
                    // Y → fullscreen preview
                    if (input.pressed(Button::Y)) {
                        mode = AppMode::PREVIEW;
                        preview.fullscreen = true;
                    }
                    // A → enter node editor for selected node
                    if (input.pressed(Button::A)) {
                        if (activeNode >= 0 && activeNode < (int)nodes.size()) {
                            nodeEditor.open(nodes[activeNode]);
                            mode = AppMode::NODE_EDITOR;
                        }
                    }
                    // X → open node browser
                    if (input.pressed(Button::X)) {
                        nodeBrowser.open();
                        mode = AppMode::NODE_BROWSER;
                    }

                    tracker.update(input, pattern);
                    break;

                case AppMode::PREVIEW:
                    if (input.pressed(Button::B) || input.pressed(Button::Y)) {
                        mode = AppMode::TRACKER;
                        preview.fullscreen = false;
                    }
                    if (input.pressed(Button::START)) {
                        pattern.togglePlaying();
                    }
                    break;

                case AppMode::NODE_EDITOR: {
                    bool back = nodeEditor.update(input);
                    if (back) {
                        nodeEditor.close();
                        mode = AppMode::TRACKER;
                    }
                    break;
                }

                case AppMode::NODE_BROWSER: {
                    int typeId = nodeBrowser.update(input);
                    if (typeId >= 0) {
                        // Create and add new node
                        VisualNode* newNode = createNode(typeId);
                        if (newNode) {
                            nodes.push_back(newNode);
                            activeNode = (int)nodes.size() - 1;
                        }
                        mode = AppMode::TRACKER;
                    } else if (nodeBrowser.cancelled()) {
                        mode = AppMode::TRACKER;
                    }
                    break;
                }

                case AppMode::PRESET_BROWSER: {
                    std::string result = presetBrowser.update(input);
                    if (!result.empty()) {
                        if (presetBrowser.isSaving()) {
                            // Extract name from path
                            std::string name = result;
                            size_t slash = name.rfind('/');
                            if (slash != std::string::npos) name = name.substr(slash + 1);
                            size_t dot = name.rfind('.');
                            if (dot != std::string::npos) name = name.substr(0, dot);
                            Preset::save(result, name, pattern.bpm(), nodes);
                        } else {
                            // Load preset
                            Preset::PresetData data = Preset::load(result);
                            if (data.valid) {
                                pattern.setBpm(data.bpm);
                                // Apply loaded params to matching nodes
                                for (int i = 0; i < (int)data.nodes.size() && i < (int)nodes.size(); i++) {
                                    for (auto& [pname, pval] : data.nodes[i].params) {
                                        nodes[i]->setParam(pname, pval);
                                    }
                                }
                            }
                        }
                        mode = AppMode::TRACKER;
                    } else if (presetBrowser.cancelled()) {
                        mode = AppMode::TRACKER;
                    }
                    break;
                }

                case AppMode::PERFORMANCE:
                    if (input.pressed(Button::B)) mode = AppMode::TRACKER;
                    break;
            }
        }

        // --- Update ---
        pattern.update(dt);

        if (pattern.isPlaying() && pattern.hasEvent()) {
            const PatternStep& ev = pattern.currentEvent();
            if (ev.nodeIndex >= 0 && ev.nodeIndex < (int)nodes.size()) {
                nodes[ev.nodeIndex]->setParam(ev.paramName, ev.value);
            }
        }

        for (auto* node : nodes) {
            if (node && node->active) {
                node->update(dt, fft.level());
            }
        }

        // --- Render ---
        renderer.beginFrame();

        switch (mode) {
            case AppMode::PREVIEW:
            case AppMode::PERFORMANCE:
                preview.render(renderer, nodes);
                break;

            case AppMode::NODE_EDITOR:
                nodeEditor.render(renderer, nodes);
                break;

            case AppMode::NODE_BROWSER:
                nodeBrowser.render(renderer);
                break;

            case AppMode::PRESET_BROWSER:
                presetBrowser.render(renderer);
                break;

            case AppMode::TRACKER:
            default:
                preview.render(renderer, nodes);
                tracker.render(renderer, pattern);

                // Status bar
                snprintf(statusBuf, sizeof(statusBuf),
                    "BPM:%3.0f  NODE:%s  %s  FRM:%d",
                    pattern.bpm(),
                    nodes[activeNode]->typeName(),
                    pattern.isPlaying() ? "PLAY" : "STOP",
                    renderer.frameCount()
                );
                renderer.rect(0, RENDER_H - 10, RENDER_W, 10, {10, 10, 16}, true);
                renderer.text(2, RENDER_H - 9, statusBuf, Palette::UI_FG);

                snprintf(statusBuf, sizeof(statusBuf), "L%d", activeNode);
                renderer.text(RENDER_W - 20, RENDER_H - 9, statusBuf, Palette::RED);
                break;
        }

        // Menu overlay (on top of everything)
        menu.render(renderer);

        renderer.endFrame();
    }

    // Cleanup heap-allocated nodes
    for (int i = staticNodeCount; i < (int)nodes.size(); i++) {
        delete nodes[i];
    }

    renderer.shutdown();
    return 0;
}
