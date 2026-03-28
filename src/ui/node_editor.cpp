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

void NodeEditor::render(Renderer& r, const std::vector<VisualNode*>& nodes) {
    if (!m_open || !m_node) return;

    int paramCount = m_node->params.count();

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    char header[64];
    snprintf(header, sizeof(header), "EDIT > %s", m_node->typeName());
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, header, Palette::RED);

    // Parameter list
    int startY = 12;
    int rowH = 10;
    int listW = RENDER_W - 8;

    for (int i = 0; i < VISIBLE_ROWS && (m_scrollOffset + i) < paramCount; i++) {
        int idx = m_scrollOffset + i;
        int py = startY + i * rowH;
        bool sel = (idx == m_cursor);

        if (sel) {
            r.rect(0, py - 1, RENDER_W, rowH, {35, 25, 35}, true);
        }

        Widgets::drawParam(r, 4, py, listW, m_node->params.at(idx), sel);
    }

    // Scrollbar
    if (paramCount > VISIBLE_ROWS) {
        int barH = VISIBLE_ROWS * rowH;
        int barY = startY;
        float ratio = (float)m_scrollOffset / std::max(1, paramCount - VISIBLE_ROWS);
        int thumbH = std::max(6, barH * VISIBLE_ROWS / paramCount);
        int thumbY = barY + (int)(ratio * (barH - thumbH));
        r.rect(RENDER_W - 3, barY, 2, barH, {30, 30, 40}, true);
        r.rect(RENDER_W - 3, thumbY, 2, thumbH, Palette::UI_FG, true);
    }

    // Mini preview at bottom
    int previewY = RENDER_H - PREVIEW_H - 10;
    r.rect(0, previewY - 1, RENDER_W, 1, Palette::GRID, true);
    r.text(4, previewY + 1, "PREVIEW", {80, 80, 90});

    // Render all nodes in the mini preview area
    // (they render at their own coordinates — this is a live view)
    // Simple: just render nodes in the bottom strip area
    // The nodes render at their configured positions so this shows a slice
    for (auto* node : nodes) {
        if (node && node->active) {
            node->render(r);
        }
    }
    // Dark overlay on non-preview area to keep param list readable
    r.rect(0, 0, RENDER_W, previewY - 1, Palette::UI_BG, true);
    // Re-render the header and params on top
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, header, Palette::RED);

    for (int i = 0; i < VISIBLE_ROWS && (m_scrollOffset + i) < paramCount; i++) {
        int idx = m_scrollOffset + i;
        int py = startY + i * rowH;
        bool sel = (idx == m_cursor);

        if (sel) {
            r.rect(0, py - 1, RENDER_W, rowH, {35, 25, 35}, true);
        }
        Widgets::drawParam(r, 4, py, listW, m_node->params.at(idx), sel);
    }

    // Re-render scrollbar
    if (paramCount > VISIBLE_ROWS) {
        int barH = VISIBLE_ROWS * rowH;
        int barY = startY;
        float ratio = (float)m_scrollOffset / std::max(1, paramCount - VISIBLE_ROWS);
        int thumbH = std::max(6, barH * VISIBLE_ROWS / paramCount);
        int thumbY = barY + (int)(ratio * (barH - thumbH));
        r.rect(RENDER_W - 3, barY, 2, barH, {30, 30, 40}, true);
        r.rect(RENDER_W - 3, thumbY, 2, thumbH, Palette::UI_FG, true);
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "L/R:NODE <>:ADJ A+<>:FINE X:DEL B:BACK", {100, 100, 110});
}
