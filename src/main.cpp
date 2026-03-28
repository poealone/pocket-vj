#include "config.h"
#include "input.h"
#include "engine/renderer.h"
#include "engine/visual_node.h"
#include "engine/layer.h"
#include "engine/sprite.h"
#include "engine/scene.h"
#include "engine/bpm.h"
#include "engine/recorder.h"
#include "engine/nodes/bars.h"
#include "engine/nodes/waveform.h"
#include "engine/nodes/shapes.h"
#include "engine/nodes/particles.h"
#include "engine/nodes/color_field.h"
#include "engine/nodes/noise.h"
#include "engine/nodes/laser.h"
#include "engine/nodes/strobe.h"
#include "engine/nodes/grid.h"
#include "engine/nodes/tunnel.h"
#include "engine/nodes/starfield.h"
#include "engine/nodes/plasma.h"
#include "engine/nodes/mirror.h"
#include "engine/nodes/blur.h"
#include "engine/nodes/feedback.h"
#include "engine/nodes/glitch.h"
#include "engine/nodes/scanline.h"
#include "engine/nodes/pixelate.h"
#include "engine/nodes/colorshift.h"
#include "engine/nodes/edge.h"
#include "engine/audio/fft.h"
#include "engine/audio/audio_input.h"
#include "engine/modulators/lfo.h"
#include "engine/modulators/envelope.h"
#include "engine/modulators/audio_mod.h"
#include "engine/3d/camera3d.h"
#include "engine/nodes/mesh.h"
#include "engine/nodes/cube.h"
#include "engine/nodes/sphere.h"
#include "engine/nodes/torus.h"
#include "sequencer/pattern.h"
#include "ui/tracker_view.h"
#include "ui/preview.h"
#include "ui/menu.h"
#include "ui/node_editor.h"
#include "ui/node_browser.h"
#include "ui/layer_editor.h"
#include "ui/file_browser.h"
#include "ui/performance.h"
#include "export/pdviz.h"
#include "preset.h"

#include <vector>
#include <cstdio>
#include <sys/stat.h>

enum class AppMode {
    TRACKER,        // Main tracker view + small preview
    PREVIEW,        // Fullscreen visual preview
    NODE_EDITOR,    // Parameter editing for selected node
    NODE_BROWSER,   // Add new node browser
    PRESET_BROWSER, // Save/load presets
    LAYER_EDITOR,   // Layer management
    FILE_BROWSER,   // File browser for sprites/assets
    PERFORMANCE     // Live FX mode (Phase 5)
};

// Presets directory path
static const char* PRESETS_DIR = "presets";

static void ensurePresetsDir() {
    mkdir(PRESETS_DIR, 0755);
}

// Create a new node by type ID (from node browser)
static VisualNode* createNode(int typeId) {
    switch (typeId) {
        case 0:  return new BarsNode();
        case 1:  return new WaveformNode();
        case 2:  return new ShapesNode();
        case 3:  return new ParticlesNode();
        case 4:  return new ColorFieldNode();
        case 5:  return new NoiseNode();
        case 6:  return new LaserNode();
        case 7:  return new StrobeNode();
        case 8:  return new GridNode();
        case 9:  return new TunnelNode();
        case 10: return new StarfieldNode();
        case 11: return new PlasmaNode();
        case 12: return new MirrorNode();
        case 13: return new BlurNode();
        case 14: return new FeedbackNode();
        case 15: return new GlitchNode();
        case 16: return new ScanlineNode();
        case 17: return new PixelateNode();
        case 18: return new ColorShiftNode();
        case 19: return new EdgeNode();
        case 20: return new LFOModulator();
        case 21: return new EnvelopeModulator();
        case 22: return new AudioModulator();
        case 23: return new MeshNode();
        case 24: return new CubeNode();
        case 25: return new SphereNode();
        case 26: return new TorusNode();
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

    // --- Audio Input ---
    AudioInput audioInput;
    bool hasAudio = audioInput.init();
    if (hasAudio) {
        SDL_Log("Audio capture initialized");
    } else {
        SDL_Log("Using demo audio mode");
    }

    ensurePresetsDir();

    // --- 3D Camera ---
    Camera3D camera3d;

    // --- Layer Manager ---
    LayerManager layers;

    // --- Create default nodes in Layer 1 ---
    layers.setCurrentLayer(0);
    layers.layer(0).name = "Background";

    ColorFieldNode* bgField = new ColorFieldNode();
    bgField->fieldType = FieldType::GRADIENT_V;
    bgField->syncParams();
    layers.addNode(bgField);

    // Layer 2: bars + wave
    layers.setCurrentLayer(1);
    layers.layer(1).name = "Audio Viz";

    BarsNode* bars = new BarsNode();
    bars->x = 0; bars->y = 180; bars->w = RENDER_W; bars->h = 60;
    bars->numBars = 16;
    bars->color = Palette::RED;
    bars->reactive = false;
    bars->syncParams();
    layers.addNode(bars);

    WaveformNode* wave = new WaveformNode();
    wave->x = 0; wave->y = 80; wave->w = RENDER_W; wave->h = 80;
    wave->color = Palette::CYAN;
    wave->reactive = false;
    wave->syncParams();
    layers.addNode(wave);

    // Layer 3: shapes + particles
    layers.setCurrentLayer(2);
    layers.layer(2).name = "Elements";

    ShapesNode* shape = new ShapesNode();
    shape->x = RENDER_W / 2 - 30; shape->y = 30;
    shape->w = 60; shape->h = 60;
    shape->shape = ShapeType::CIRCLE;
    shape->color = Palette::MAGENTA;
    shape->syncParams();
    layers.addNode(shape);

    ParticlesNode* particles = new ParticlesNode();
    particles->x = RENDER_W / 2;
    particles->y = 120;
    particles->color = Palette::YELLOW;
    particles->reactive = false;
    particles->syncParams();
    layers.addNode(particles);

    // Reset to layer 1
    layers.setCurrentLayer(0);

    // Get flat node list for compatibility
    auto allNodes = layers.allNodes();

    // --- Sequencer ---
    Pattern pattern(16);
    pattern.setBpm(120.0f);

    // --- Phase 5: Scene, BPM, Recorder, Performance ---
    SceneManager sceneManager;
    BpmTap bpmTap;
    Recorder recorder;
    PerformanceMode perfMode;

    // --- UI ---
    TrackerView tracker;
    PreviewPanel preview;
    Menu menu;
    NodeEditor nodeEditor;
    NodeBrowser nodeBrowser;
    PresetBrowser presetBrowser;
    LayerEditor layerEditor;
    FileBrowser fileBrowser;
    AppMode mode = AppMode::TRACKER;

    // Scene management sub-state
    bool sceneMenuOpen = false;
    int sceneMenuCursor = 0;

    char statusBuf[128];

    // --- Main Loop ---
    bool running = true;
    while (running) {
        input.poll();
        if (input.quit() || renderer.shouldQuit()) break;

        float dt = renderer.deltaTime();

        // --- Audio + FFT ---
        audioInput.update(fft);
        if (!fft.hasAudioInput()) {
            fft.generateDemo(dt);
        }
        bars->setFFTData(fft.bins(), fft.binCount());

        // Refresh flat node list (in case nodes were added/removed)
        allNodes = layers.allNodes();

        // --- Feed audio modulators ---
        for (auto* node : allNodes) {
            AudioModulator* amod = dynamic_cast<AudioModulator*>(node);
            if (amod && amod->active) {
                amod->feed(fft);
            }
        }

        // --- Handle mode-specific input ---
        if (sceneMenuOpen) {
            // Scene management overlay
            if (input.pressed(Button::UP)) {
                sceneMenuCursor = (sceneMenuCursor - 1 + MAX_SCENES) % MAX_SCENES;
            }
            if (input.pressed(Button::DOWN)) {
                sceneMenuCursor = (sceneMenuCursor + 1) % MAX_SCENES;
            }
            if (input.pressed(Button::A)) {
                // Save current state to this scene slot
                sceneManager.saveScene(sceneMenuCursor, layers);
            }
            if (input.pressed(Button::X)) {
                // Load this scene slot
                if (sceneManager.hasScene(sceneMenuCursor)) {
                    sceneManager.loadScene(sceneMenuCursor, layers);
                }
            }
            if (input.pressed(Button::B)) {
                sceneMenuOpen = false;
            }
        } else if (menu.isOpen()) {
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
                    case 2: // EXPORT .PDVIZ
                    {
                        ensurePresetsDir();
                        std::string pdvizPath = std::string(PRESETS_DIR) + "/export.pdviz";
                        PdViz::exportFile(pdvizPath, "Pocket VJ Export", layers);
                        break;
                    }
                    case 3: // PERFORMANCE MODE
                        perfMode.init(&layers, &sceneManager, &bpmTap, &pattern, &recorder);
                        mode = AppMode::PERFORMANCE;
                        break;
                    case 4: // RECORD
                        if (recorder.isRecording()) {
                            recorder.stopRecording();
                            ensurePresetsDir();
                            recorder.saveRecording(std::string(PRESETS_DIR) + "/recording.pvjr");
                        } else {
                            recorder.startRecording();
                        }
                        break;
                    case 5: // SCENES
                        sceneMenuOpen = true;
                        sceneMenuCursor = sceneManager.currentScene();
                        break;
                    case 8: // EXIT (was 5, now 8)
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
                        // Check if Start is also held → enter performance mode
                        if (input.held(Button::START)) {
                            perfMode.init(&layers, &sceneManager, &bpmTap, &pattern, &recorder);
                            mode = AppMode::PERFORMANCE;
                        } else {
                            menu.open();
                        }
                    }
                    if (input.pressed(Button::START)) {
                        if (!input.held(Button::SELECT)) {
                            pattern.togglePlaying();
                        }
                    }
                    // L/R → switch active layer
                    if (input.pressed(Button::L)) {
                        int cur = layers.currentLayer();
                        cur = (cur - 1 + layers.layerCount()) % layers.layerCount();
                        layers.setCurrentLayer(cur);
                    }
                    if (input.pressed(Button::R)) {
                        int cur = layers.currentLayer();
                        cur = (cur + 1) % layers.layerCount();
                        layers.setCurrentLayer(cur);
                    }
                    // Y → fullscreen preview
                    if (input.pressed(Button::Y)) {
                        mode = AppMode::PREVIEW;
                        preview.fullscreen = true;
                    }
                    // A → open layer editor
                    if (input.pressed(Button::A)) {
                        layerEditor.open(&layers);
                        mode = AppMode::LAYER_EDITOR;
                    }
                    // X → open node browser (adds to current layer)
                    if (input.pressed(Button::X)) {
                        nodeBrowser.open();
                        mode = AppMode::NODE_BROWSER;
                    }

                    // L2 → toggle recording
                    if (input.pressed(Button::L2)) {
                        if (recorder.isRecording()) {
                            recorder.stopRecording();
                            ensurePresetsDir();
                            recorder.saveRecording(std::string(PRESETS_DIR) + "/recording.pvjr");
                        } else {
                            recorder.startRecording();
                        }
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

                case AppMode::LAYER_EDITOR: {
                    int enterLayer = -1;
                    bool back = layerEditor.update(input, enterLayer);
                    if (enterLayer >= 0) {
                        // Enter node editor for first node in that layer
                        layers.setCurrentLayer(enterLayer);
                        auto& nodes = layers.layer(enterLayer).nodes;
                        if (!nodes.empty()) {
                            nodeEditor.open(nodes[0]);
                            mode = AppMode::NODE_EDITOR;
                        } else {
                            // No nodes — open node browser to add one
                            nodeBrowser.open();
                            mode = AppMode::NODE_BROWSER;
                        }
                        layerEditor.close();
                    }
                    if (back) {
                        mode = AppMode::TRACKER;
                    }
                    break;
                }

                case AppMode::NODE_EDITOR: {
                    bool back = nodeEditor.update(input);
                    if (back) {
                        nodeEditor.close();
                        // Go back to layer editor
                        layerEditor.open(&layers);
                        mode = AppMode::LAYER_EDITOR;
                    }
                    break;
                }

                case AppMode::NODE_BROWSER: {
                    int typeId = nodeBrowser.update(input);
                    if (typeId >= 0) {
                        // Create and add new node to current layer
                        VisualNode* newNode = createNode(typeId);
                        if (newNode) {
                            // Inject camera into 3D nodes
                            if (auto* m = dynamic_cast<MeshNode*>(newNode))   m->setCamera(&camera3d);
                            if (auto* m = dynamic_cast<CubeNode*>(newNode))   m->setCamera(&camera3d);
                            if (auto* m = dynamic_cast<SphereNode*>(newNode)) m->setCamera(&camera3d);
                            if (auto* m = dynamic_cast<TorusNode*>(newNode))  m->setCamera(&camera3d);
                            layers.addNode(newNode);
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
                            std::string name = result;
                            size_t slash = name.rfind('/');
                            if (slash != std::string::npos) name = name.substr(slash + 1);
                            size_t dot = name.rfind('.');
                            if (dot != std::string::npos) name = name.substr(0, dot);
                            Preset::save(result, name, pattern.bpm(), allNodes);
                        } else {
                            Preset::PresetData data = Preset::load(result);
                            if (data.valid) {
                                pattern.setBpm(data.bpm);
                                for (int i = 0; i < (int)data.nodes.size() && i < (int)allNodes.size(); i++) {
                                    for (auto& [pname, pval] : data.nodes[i].params) {
                                        allNodes[i]->setParam(pname, pval);
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

                case AppMode::FILE_BROWSER: {
                    std::string result = fileBrowser.update(input);
                    if (!result.empty()) {
                        mode = AppMode::TRACKER;
                    } else if (fileBrowser.cancelled()) {
                        mode = AppMode::TRACKER;
                    }
                    break;
                }

                case AppMode::PERFORMANCE: {
                    bool exitPerf = perfMode.update(input);
                    if (exitPerf) {
                        mode = AppMode::TRACKER;
                    }

                    // L2 → toggle recording in performance mode too
                    if (input.pressed(Button::L2)) {
                        if (recorder.isRecording()) {
                            recorder.stopRecording();
                            ensurePresetsDir();
                            recorder.saveRecording(std::string(PRESETS_DIR) + "/recording.pvjr");
                        } else {
                            recorder.startRecording();
                        }
                    }
                    break;
                }
            }
        }

        // --- Update ---
        pattern.update(dt);

        if (pattern.isPlaying() && pattern.hasEvent()) {
            const PatternStep& ev = pattern.currentEvent();
            if (ev.nodeIndex >= 0 && ev.nodeIndex < (int)allNodes.size()) {
                allNodes[ev.nodeIndex]->setParam(ev.paramName, ev.value);
            }
        }

        // Update 3D camera
        camera3d.update(dt);

        // Update all nodes via layer manager
        layers.updateAll(dt, fft.level());

        // --- Render ---
        renderer.beginFrame();

        switch (mode) {
            case AppMode::PREVIEW:
                layers.renderAll(renderer);
                break;

            case AppMode::PERFORMANCE:
                layers.renderAll(renderer);
                perfMode.renderOverlay(renderer);
                break;

            case AppMode::LAYER_EDITOR:
                layers.renderAll(renderer);
                layerEditor.render(renderer);
                break;

            case AppMode::NODE_EDITOR:
                nodeEditor.render(renderer, allNodes);
                break;

            case AppMode::NODE_BROWSER:
                nodeBrowser.render(renderer);
                break;

            case AppMode::PRESET_BROWSER:
                presetBrowser.render(renderer);
                break;

            case AppMode::FILE_BROWSER:
                fileBrowser.render(renderer);
                break;

            case AppMode::TRACKER:
            default:
                // Use layer manager for rendering
                layers.renderAll(renderer);
                tracker.render(renderer, pattern);

                // Status bar
                snprintf(statusBuf, sizeof(statusBuf),
                    "BPM:%3.0f  LYR:%d/%s  %s  FRM:%d",
                    pattern.bpm(),
                    layers.currentLayer() + 1,
                    layers.layer(layers.currentLayer()).name.c_str(),
                    pattern.isPlaying() ? "PLAY" : "STOP",
                    renderer.frameCount()
                );
                renderer.rect(0, RENDER_H - 10, RENDER_W, 10, {10, 10, 16}, true);
                renderer.text(2, RENDER_H - 9, statusBuf, Palette::UI_FG);

                // Recording indicator
                if (recorder.isRecording()) {
                    renderer.circle(RENDER_W - 8, RENDER_H - 5, 3, {255, 0, 0}, true);
                }
                break;
        }

        // Scene management overlay (drawn on top of everything if open)
        if (sceneMenuOpen) {
            int sx = 50, sy = 30, sw = 220, sh = 180;
            renderer.rect(sx, sy, sw, sh, {10, 10, 16}, true);
            renderer.rect(sx, sy, sw, sh, Palette::UI_FG, false);
            renderer.textCentered(sy + 4, "SCENES", Palette::RED);
            renderer.line(sx + 8, sy + 16, sx + sw - 8, sy + 16, Palette::GRID);

            for (int i = 0; i < MAX_SCENES; i++) {
                int iy = sy + 22 + i * 16;
                bool sel = (i == sceneMenuCursor);
                bool hasData = sceneManager.hasScene(i);

                if (sel) {
                    renderer.rect(sx + 2, iy - 2, sw - 4, 14, {40, 20, 30}, true);
                }

                char sbuf[64];
                snprintf(sbuf, sizeof(sbuf), "%s %d: %s",
                         sel ? ">" : " ", i + 1,
                         hasData ? sceneManager.scene(i).name.c_str() : "[empty]");
                Color sc = sel ? Palette::RED : (hasData ? Palette::UI_FG : Palette::GRID);
                renderer.text(sx + 8, iy, sbuf, sc);

                if (hasData && sceneManager.currentScene() == i) {
                    renderer.text(sx + sw - 24, iy, "<-", Palette::CYAN);
                }
            }

            renderer.rect(sx, sy + sh - 14, sw, 14, {10, 10, 16}, true);
            renderer.text(sx + 4, sy + sh - 12, "A:SAVE  X:LOAD  B:BACK", Palette::GRID);
        }

        // Menu overlay (on top of everything)
        menu.render(renderer);

        // Capture frame for recording (after all rendering)
        if (recorder.isRecording()) {
            recorder.captureFrame(renderer.pixels());
        }

        renderer.endFrame();
    }

    // Cleanup
    audioInput.shutdown();

    // Cleanup all heap-allocated nodes from layers
    for (int i = 0; i < layers.layerCount(); i++) {
        for (auto* node : layers.layer(i).nodes) {
            delete node;
        }
    }

    renderer.shutdown();
    return 0;
}
