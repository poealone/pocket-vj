#include "node_browser.h"
#include <algorithm>

// Node type IDs — these map to actual node types we can create
enum NodeTypeId {
    NT_BARS = 0,
    NT_WAVE,
    NT_SHAPE,
    NT_PARTICLE,
    NT_FIELD,
    NT_NOISE,
    NT_LASER,
    NT_STROBE,
    NT_GRID,
    NT_TUNNEL,
    NT_STARFIELD,
    NT_PLASMA,
    NT_MIRROR,
    NT_BLUR,
    NT_FEEDBACK,
    NT_GLITCH,
    NT_SCANLINE,
    NT_PIXELATE,
    NT_COLORSHIFT,
    NT_EDGE,
    NT_LFO,
    NT_ENVELOPE,
    NT_AUDIOMOD,
    NT_MESH,
    NT_CUBE,
    NT_SPHERE,
    NT_TORUS,
    NT_COUNT
};

NodeBrowser::NodeBrowser() {
    // Populate the browser entries with all available nodes
    m_entries = {
        // Source nodes
        {"BARS",     "Spectrum analyzer bars",    NodeCategory::SOURCE, NT_BARS},
        {"WAVE",     "Oscilloscope waveform",     NodeCategory::SOURCE, NT_WAVE},
        {"SHAPE",    "Geometric primitives",      NodeCategory::SOURCE, NT_SHAPE},
        {"PARTICLE", "Particle emitter",          NodeCategory::SOURCE, NT_PARTICLE},
        {"FIELD",    "Color fill / gradient",     NodeCategory::SOURCE, NT_FIELD},
        {"NOISE",    "Procedural noise pattern",  NodeCategory::SOURCE, NT_NOISE},
        {"LASER",    "Vector line patterns",      NodeCategory::SOURCE, NT_LASER},
        {"STROBE",   "Flash/pulse strobe",        NodeCategory::SOURCE, NT_STROBE},
        {"GRID",     "Animated dot grid",         NodeCategory::SOURCE, NT_GRID},
        {"TUNNEL",   "Infinite tunnel effect",    NodeCategory::SOURCE, NT_TUNNEL},
        {"STAR",     "3D star field",             NodeCategory::SOURCE, NT_STARFIELD},
        {"PLASMA",   "Plasma sine waves",         NodeCategory::SOURCE, NT_PLASMA},
        // FX nodes
        {"MIRROR",   "Mirror framebuffer",        NodeCategory::FX, NT_MIRROR},
        {"BLUR",     "Box blur effect",           NodeCategory::FX, NT_BLUR},
        {"FEEDBACK", "Frame feedback / trails",   NodeCategory::FX, NT_FEEDBACK},
        {"GLITCH",   "Digital glitch effect",     NodeCategory::FX, NT_GLITCH},
        {"SCANLINE", "CRT scanline overlay",      NodeCategory::FX, NT_SCANLINE},
        {"PIXELATE", "Mosaic / big pixel",        NodeCategory::FX, NT_PIXELATE},
        {"COLORSHF", "Hue/sat/brightness shift",  NodeCategory::FX, NT_COLORSHIFT},
        {"EDGE",     "Sobel edge detection",      NodeCategory::FX, NT_EDGE},
        // Modulator nodes
        {"LFO",      "LFO modulator",             NodeCategory::MODULATOR, NT_LFO},
        {"ENV",      "ADSR envelope",             NodeCategory::MODULATOR, NT_ENVELOPE},
        {"AMOD",     "Audio-reactive mod",        NodeCategory::MODULATOR, NT_AUDIOMOD},
        // 3D nodes
        {"MESH",     "3D mesh from .obj file",    NodeCategory::THREED, NT_MESH},
        {"CUBE",     "3D rotating cube",          NodeCategory::THREED, NT_CUBE},
        {"SPHERE",   "3D UV sphere",              NodeCategory::THREED, NT_SPHERE},
        {"TORUS",    "3D torus / donut",          NodeCategory::THREED, NT_TORUS},
    };
}

void NodeBrowser::open() {
    m_open = true;
    m_cancelled = false;
    m_cursor = 0;
    m_scrollOffset = 0;
}

void NodeBrowser::close() {
    m_open = false;
}

int NodeBrowser::update(Input& input) {
    if (!m_open) return -1;

    int count = (int)m_entries.size();

    if (input.pressed(Button::UP)) {
        m_cursor--;
        if (m_cursor < 0) m_cursor = count - 1;
    }
    if (input.pressed(Button::DOWN)) {
        m_cursor++;
        if (m_cursor >= count) m_cursor = 0;
    }

    // Scroll
    if (m_cursor < m_scrollOffset) m_scrollOffset = m_cursor;
    if (m_cursor >= m_scrollOffset + VISIBLE_ROWS) m_scrollOffset = m_cursor - VISIBLE_ROWS + 1;

    // Select
    if (input.pressed(Button::A)) {
        int id = m_entries[m_cursor].nodeTypeId;
        close();
        return id;
    }

    // Cancel
    if (input.pressed(Button::B)) {
        m_cancelled = true;
        close();
        return -1;
    }

    return -1;
}

void NodeBrowser::render(Renderer& r) {
    if (!m_open) return;

    int count = (int)m_entries.size();

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, "ADD NODE", Palette::RED);

    // Category headers + entries
    int startY = 12;
    int rowH = 10;
    NodeCategory lastCat = (NodeCategory)-1;
    int drawY = startY;
    int visIdx = 0;

    for (int i = m_scrollOffset; i < count && visIdx < VISIBLE_ROWS; i++) {
        const auto& entry = m_entries[i];

        // Category header
        if (entry.category != lastCat) {
            lastCat = entry.category;
            const char* catName = "???";
            switch (entry.category) {
                case NodeCategory::SOURCE:    catName = "-- SOURCE --"; break;
                case NodeCategory::TRANSFORM: catName = "-- TRANSFORM --"; break;
                case NodeCategory::FX:        catName = "-- FX --"; break;
                case NodeCategory::MODULATOR: catName = "-- MODULATOR --"; break;
                case NodeCategory::THREED:    catName = "-- 3D --"; break;
            }
            // Only show category header if it's the first entry of this cat in view
            if (i == m_scrollOffset || m_entries[i - 1].category != entry.category) {
                r.text(4, drawY, catName, Palette::YELLOW);
                drawY += rowH;
                visIdx++;
                if (visIdx >= VISIBLE_ROWS) break;
            }
        }

        bool sel = (i == m_cursor);
        if (sel) {
            r.rect(0, drawY - 1, RENDER_W, rowH, {40, 25, 35}, true);
        }

        // Node name
        r.text(10, drawY, entry.name, sel ? Palette::RED : Palette::WHITE);

        // Description (right-aligned-ish)
        int descX = 80;
        std::string desc = entry.description;
        if (desc.size() > 28) desc = desc.substr(0, 28);
        r.text(descX, drawY, desc, {100, 100, 120});

        drawY += rowH;
        visIdx++;
    }

    // Description of selected node at bottom
    if (m_cursor >= 0 && m_cursor < count) {
        r.rect(0, RENDER_H - 20, RENDER_W, 11, {20, 20, 30}, true);
        r.text(4, RENDER_H - 18, m_entries[m_cursor].description, Palette::CYAN);
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "A:ADD  B:CANCEL  UP/DN:BROWSE", {100, 100, 110});
}
