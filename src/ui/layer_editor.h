#pragma once
#include "../engine/renderer.h"
#include "../engine/layer.h"
#include "../input.h"

class LayerEditor {
public:
    void open(LayerManager* mgr);
    void close();
    bool isOpen() const { return m_open; }

    // Returns true if user pressed B (back)
    // Sets enterLayer to layer index if user pressed A to enter a layer (-1 otherwise)
    bool update(Input& input, int& enterLayer);
    void render(Renderer& r);

private:
    bool m_open = false;
    LayerManager* m_mgr = nullptr;
    int m_cursor = 0;
    bool m_adjustingOpacity = false;
};
