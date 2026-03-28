#pragma once
#include "../engine/renderer.h"
#include "../engine/visual_node.h"
#include "../engine/layer.h"
#include "../input.h"
#include <vector>

class NodeEditor {
public:
    // Set the node to edit. Syncs params from node state.
    void open(VisualNode* node, LayerManager* layers = nullptr);
    void close();
    bool isOpen() const { return m_open; }

    // Returns true if user pressed B (wants to go back)
    bool update(Input& input);

    // Render the editor UI + inline preview
    void render(Renderer& r, const std::vector<VisualNode*>& nodes);

    VisualNode* currentNode() const { return m_node; }

    bool deleteRequested() const { return m_deleteRequested; }

private:
    bool m_open = false;
    bool m_deleteRequested = false;
    VisualNode* m_node = nullptr;
    LayerManager* m_layers = nullptr;
    int  m_cursor = 0;       // Selected param index
    int  m_scrollOffset = 0;
    static const int VISIBLE_ROWS = 10;
    static const int PREVIEW_H = 50;  // Mini preview height at bottom
};
