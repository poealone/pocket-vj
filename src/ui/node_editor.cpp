#include "node_editor.h"
#include "widgets.h"
#include <cstdio>
#include <algorithm>

void NodeEditor::open(VisualNode* node, LayerManager* layers) {
    m_node = node;
    m_layers = layers;
    m_open = true;
    m_deleteRequested = false;
    m_cursor = 0;
    m_scrollOffset = 0;
    if (m_node) {
        m_node->syncParams();
    }
}

void NodeEditor::close() {
    m_open = false;
    m_node = nullptr;
}

bool NodeEditor::update(Input& input) {
    if (!m_open || !m_node) return false;

    int paramCount = m_node->params.count();
    if (paramCount == 0) return false;

    // Navigation
    if (input.pressed(Button::UP)) {
        m_cursor--;
        if (m_cursor < 0) m_cursor = paramCount - 1;
    }
    if (input.pressed(Button::DOWN)) {
        m_cursor++;
        if (m_cursor >= paramCount) m_cursor = 0;
    }

    // Keep cursor in scroll view
    if (m_cursor < m_scrollOffset) m_scrollOffset = m_cursor;
    if (m_cursor >= m_scrollOffset + VISIBLE_ROWS) m_scrollOffset = m_cursor - VISIBLE_ROWS + 1;

    // Adjust values
    Param& p = m_node->params.at(m_cursor);

    bool fineMode = input.held(Button::A);

    if (p.type == ParamType::TOGGLE) {
        // Toggle on A press or Left/Right
        if (input.pressed(Button::A) || input.pressed(Button::LEFT) || input.pressed(Button::RIGHT)) {
            p.toggle();
            m_node->applyParams();
        }
    } else if (p.type == ParamType::ENUM) {
        if (input.pressed(Button::LEFT)) {
            p.adjust(-1.0f);
            m_node->applyParams();
        }
        if (input.pressed(Button::RIGHT)) {
            p.adjust(1.0f);
            m_node->applyParams();
        }
    } else {
        // FLOAT / INT
        float delta = p.step;
        if (fineMode) delta = p.step / 10.0f;

        if (input.pressed(Button::LEFT)) {
            p.adjust(-delta);
            m_node->applyParams();
        }
        if (input.pressed(Button::RIGHT)) {
            p.adjust(delta);
            m_node->applyParams();
        }

    }

    // L/R: switch to prev/next node in current layer
    if (m_layers && (input.pressed(Button::L) || input.pressed(Button::R))) {
        auto& nodes = m_layers->currentNodes();
        if (nodes.size() > 1) {
            int idx = -1;
            for (int i = 0; i < (int)nodes.size(); i++) {
                if (nodes[i] == m_node) { idx = i; break; }
            }
            if (idx >= 0) {
                if (input.pressed(Button::R)) idx = (idx + 1) % (int)nodes.size();
                else idx = (idx - 1 + (int)nodes.size()) % (int)nodes.size();
                open(nodes[idx], m_layers);
            }
        }
    }

    // X = delete current node
    if (input.pressed(Button::X)) {
        m_deleteRequested = true;
        return true;
    }

    // B = back
    if (input.pressed(Button::B)) {
        return true;
    }

    return false;
}

void NodeEditor::render(Renderer& r, const std::vector<VisualNode*>& /*nodes*/) {
    if (!m_open || !m_node) return;

    int paramCount = m_node->params.count();

    // Visuals are rendered BEFORE this call (in main.cpp) — we just draw the overlay

    // Semi-transparent panel on the left side (half screen width)
    int panelW = 200;
    int panelH = RENDER_H;

    // Darken the left panel area for readability
    r.rectAlpha(0, 0, panelW, panelH, {0, 0, 0, 180});

    // Header bar
    char header[64];
    snprintf(header, sizeof(header), "%s", m_node->typeName());
    r.rectAlpha(0, 0, panelW, 11, {20, 10, 30, 220});
    r.text(4, 2, header, Palette::RED);

    // Node index indicator (if in a layer with multiple nodes)
    if (m_layers) {
        auto& lnodes = m_layers->currentNodes();
        int nodeIdx = -1;
        for (int i = 0; i < (int)lnodes.size(); i++) {
            if (lnodes[i] == m_node) { nodeIdx = i; break; }
        }
        if (nodeIdx >= 0 && lnodes.size() > 1) {
            char idxBuf[16];
            snprintf(idxBuf, sizeof(idxBuf), "%d/%d", nodeIdx + 1, (int)lnodes.size());
            r.text(panelW - 30, 2, idxBuf, Palette::CYAN);
        }
    }

    // Active toggle indicator
    r.text(panelW - 50, 2, m_node->active ? "ON" : "OFF",
           m_node->active ? Color(0, 255, 0) : Color(255, 60, 60));

    // Parameter list
    int startY = 14;
    int rowH = 11;
    int listW = panelW - 8;
    int maxVisible = (RENDER_H - startY - 12) / rowH;
    if (maxVisible > paramCount) maxVisible = paramCount;

    for (int i = 0; i < maxVisible && (m_scrollOffset + i) < paramCount; i++) {
        int idx = m_scrollOffset + i;
        int py = startY + i * rowH;
        bool sel = (idx == m_cursor);

        if (sel) {
            r.rectAlpha(0, py - 1, panelW, rowH, {60, 20, 40, 200});
        }

        Widgets::drawParam(r, 4, py, listW, m_node->params.at(idx), sel);
    }

    // Scrollbar (if needed)
    if (paramCount > maxVisible) {
        int barH = maxVisible * rowH;
        int barY = startY;
        float ratio = (float)m_scrollOffset / std::max(1, paramCount - maxVisible);
        int thumbH = std::max(6, barH * maxVisible / paramCount);
        int thumbY = barY + (int)(ratio * (barH - thumbH));
        r.rect(panelW - 3, barY, 2, barH, {30, 30, 40}, true);
        r.rect(panelW - 3, thumbY, 2, thumbH, Palette::UI_FG, true);
    }

    // Help bar at bottom
    r.rectAlpha(0, RENDER_H - 10, panelW, 10, {0, 0, 0, 220});
    r.text(2, RENDER_H - 9, "LR:NOD <>:ADJ X:DEL B:BACK", {100, 100, 120});
}
