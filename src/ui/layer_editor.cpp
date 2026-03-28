#include "layer_editor.h"
#include <cstdio>
#include <algorithm>

void LayerEditor::open(LayerManager* mgr) {
    m_open = true;
    m_mgr = mgr;
    m_cursor = mgr->currentLayer();
    m_adjustingOpacity = false;
}

void LayerEditor::close() {
    m_open = false;
    m_adjustingOpacity = false;
}

bool LayerEditor::update(Input& input, int& enterLayer) {
    enterLayer = -1;
    if (!m_open || !m_mgr) return false;

    int layerCount = m_mgr->layerCount();

    if (m_adjustingOpacity) {
        // In opacity adjust mode: left/right changes opacity, any other button exits
        Layer& layer = m_mgr->layer(m_cursor);
        if (input.pressed(Button::LEFT)) {
            layer.opacity = std::max(0.0f, layer.opacity - 0.05f);
        }
        if (input.pressed(Button::RIGHT)) {
            layer.opacity = std::min(1.0f, layer.opacity + 0.05f);
        }
        if (input.pressed(Button::Y) || input.pressed(Button::B) || input.pressed(Button::A)) {
            m_adjustingOpacity = false;
        }
        return false;
    }

    // Navigation
    if (input.pressed(Button::UP)) {
        m_cursor--;
        if (m_cursor < 0) m_cursor = layerCount - 1;
    }
    if (input.pressed(Button::DOWN)) {
        m_cursor++;
        if (m_cursor >= layerCount) m_cursor = 0;
    }

    // A: enter layer (edit nodes)
    if (input.pressed(Button::A)) {
        m_mgr->setCurrentLayer(m_cursor);
        enterLayer = m_cursor;
        return false;
    }

    // X: cycle blend mode
    if (input.pressed(Button::X)) {
        Layer& layer = m_mgr->layer(m_cursor);
        layer.blend = LayerManager::nextBlendMode(layer.blend);
    }

    // Y: adjust opacity mode
    if (input.pressed(Button::Y)) {
        m_adjustingOpacity = true;
        return false;
    }

    // L: toggle mute
    if (input.pressed(Button::L)) {
        Layer& layer = m_mgr->layer(m_cursor);
        layer.mute = !layer.mute;
    }

    // R: toggle solo
    if (input.pressed(Button::R)) {
        Layer& layer = m_mgr->layer(m_cursor);
        layer.solo = !layer.solo;
    }

    // B: back
    if (input.pressed(Button::B)) {
        close();
        return true;
    }

    return false;
}

void LayerEditor::render(Renderer& r) {
    if (!m_open || !m_mgr) return;

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, "LAYERS", Palette::RED);

    if (m_adjustingOpacity) {
        r.text(100, 1, "[OPACITY: L/R]", Palette::YELLOW);
    }

    int startY = 14;
    int rowH = 26;

    for (int i = 0; i < m_mgr->layerCount(); i++) {
        const Layer& layer = m_mgr->layer(i);
        bool sel = (i == m_cursor);
        int drawY = startY + i * rowH;

        if (drawY + rowH > RENDER_H - 10) break;

        // Selection highlight
        if (sel) {
            r.rect(0, drawY - 1, RENDER_W, rowH, {35, 20, 30}, true);
        }

        // Layer number
        char numBuf[8];
        snprintf(numBuf, sizeof(numBuf), "%d", i + 1);
        r.text(4, drawY + 2, numBuf, sel ? Palette::RED : Palette::WHITE);

        // Mute/Solo indicators
        Color muteColor = layer.mute ? Color(255, 60, 60) : Color(60, 60, 80);
        Color soloColor = layer.solo ? Color(255, 220, 30) : Color(60, 60, 80);
        r.text(16, drawY + 2, layer.mute ? "M" : "m", muteColor);
        r.text(24, drawY + 2, layer.solo ? "S" : "s", soloColor);

        // Activity bar (mini visualization of node count)
        int barX = 36;
        int barW = 60;
        int barH = 6;
        r.rect(barX, drawY + 3, barW, barH, {30, 30, 40}, true);
        int fillW = std::min(barW, (int)layer.nodes.size() * 8);
        if (fillW > 0) {
            Color barColor = layer.mute ? Color(80, 40, 40) : Palette::CYAN;
            r.rect(barX, drawY + 3, fillW, barH, barColor, true);
        }

        // Layer name / node summary
        char nameBuf[32];
        if (layer.nodes.empty()) {
            snprintf(nameBuf, sizeof(nameBuf), "(empty)");
        } else if (layer.nodes.size() == 1) {
            snprintf(nameBuf, sizeof(nameBuf), "%s", layer.nodes[0]->typeName());
        } else {
            snprintf(nameBuf, sizeof(nameBuf), "%s+%d",
                layer.nodes[0]->typeName(), (int)layer.nodes.size() - 1);
        }
        r.text(100, drawY + 2, nameBuf, {150, 150, 170});

        // Blend mode
        r.text(170, drawY + 2, LayerManager::blendModeName(layer.blend),
               sel ? Palette::YELLOW : Color(120, 120, 140));

        // Opacity
        char opBuf[8];
        snprintf(opBuf, sizeof(opBuf), "%3d%%", (int)(layer.opacity * 100));
        r.text(185, drawY + 2, opBuf, sel ? Palette::WHITE : Color(120, 120, 140));

        // Second line: node type names
        if (!layer.nodes.empty()) {
            std::string nodeList;
            for (size_t j = 0; j < layer.nodes.size() && j < 6; j++) {
                if (j > 0) nodeList += " ";
                nodeList += layer.nodes[j]->typeName();
            }
            if (layer.nodes.size() > 6) nodeList += "...";
            r.text(36, drawY + 12, nodeList, {80, 80, 100});
        }
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "A:EDIT X:BLEND Y:OPAC L:MUTE R:SOLO B:BACK", {100, 100, 110});
}
