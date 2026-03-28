#include "layer.h"
#include <cstring>
#include <algorithm>

LayerManager::LayerManager() {
    m_layerBuf = new uint32_t[RENDER_W * RENDER_H];
    m_compositeBuf = new uint32_t[RENDER_W * RENDER_H];

    // Initialize layer names
    for (int i = 0; i < MAX_LAYERS; i++) {
        m_layers[i].name = "Layer " + std::to_string(i + 1);
    }
}

LayerManager::~LayerManager() {
    delete[] m_layerBuf;
    delete[] m_compositeBuf;
}

void LayerManager::setCurrentLayer(int idx) {
    if (idx >= 0 && idx < MAX_LAYERS) {
        m_current = idx;
    }
}

void LayerManager::addNode(VisualNode* node) {
    if (node) {
        m_layers[m_current].nodes.push_back(node);
    }
}

void LayerManager::removeNode(VisualNode* node) {
    for (int i = 0; i < MAX_LAYERS; i++) {
        auto& nodes = m_layers[i].nodes;
        auto it = std::find(nodes.begin(), nodes.end(), node);
        if (it != nodes.end()) {
            nodes.erase(it);
            return;
        }
    }
}

std::vector<VisualNode*> LayerManager::allNodes() const {
    std::vector<VisualNode*> all;
    for (int i = 0; i < MAX_LAYERS; i++) {
        for (auto* n : m_layers[i].nodes) {
            all.push_back(n);
        }
    }
    return all;
}

void LayerManager::updateAll(float dt, float audioLevel) {
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (m_layers[i].mute) continue;
        for (auto* node : m_layers[i].nodes) {
            if (node && node->active) {
                node->update(dt, audioLevel);
            }
        }
    }
}

void LayerManager::renderAll(Renderer& r) {
    // Check if any layer is solo'd
    bool anySolo = false;
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (m_layers[i].solo && !m_layers[i].mute) {
            anySolo = true;
            break;
        }
    }

    // Clear composite buffer
    memset(m_compositeBuf, 0, RENDER_W * RENDER_H * sizeof(uint32_t));

    bool firstLayer = true;

    // Render layers bottom to top
    for (int i = 0; i < MAX_LAYERS; i++) {
        const Layer& layer = m_layers[i];

        // Skip conditions
        if (layer.mute) continue;
        if (anySolo && !layer.solo) continue;
        if (layer.nodes.empty()) continue;
        if (layer.opacity <= 0.0f) continue;

        // Render layer nodes to the renderer's pixel buffer
        // Save current pixels, clear, render layer, then composite
        uint32_t* px = r.pixels();

        // Clear renderer for this layer
        memset(px, 0, RENDER_W * RENDER_H * sizeof(uint32_t));

        // Render all active nodes in this layer
        for (auto* node : layer.nodes) {
            if (node && node->active) {
                node->render(r);
            }
        }

        // Copy rendered layer to layerBuf
        memcpy(m_layerBuf, px, RENDER_W * RENDER_H * sizeof(uint32_t));

        if (firstLayer) {
            // First visible layer: just copy with opacity
            if (layer.opacity >= 1.0f && layer.blend == BlendMode::NORMAL) {
                memcpy(m_compositeBuf, m_layerBuf, RENDER_W * RENDER_H * sizeof(uint32_t));
            } else {
                compositeLayer(m_layerBuf, m_compositeBuf, layer.blend, layer.opacity);
            }
            firstLayer = false;
        } else {
            // Composite this layer onto the accumulator
            compositeLayer(m_layerBuf, m_compositeBuf, layer.blend, layer.opacity);
        }
    }

    // Copy final composite to renderer
    memcpy(r.pixels(), m_compositeBuf, RENDER_W * RENDER_H * sizeof(uint32_t));
}

void LayerManager::compositeLayer(const uint32_t* src, uint32_t* dst, BlendMode mode, float opacity) {
    for (int i = 0; i < RENDER_W * RENDER_H; i++) {
        switch (mode) {
            case BlendMode::NORMAL:   dst[i] = blendNormal(src[i], dst[i], opacity); break;
            case BlendMode::ADD:      dst[i] = blendAdd(src[i], dst[i], opacity); break;
            case BlendMode::MULTIPLY: dst[i] = blendMultiply(src[i], dst[i], opacity); break;
            case BlendMode::SCREEN:   dst[i] = blendScreen(src[i], dst[i], opacity); break;
            case BlendMode::OVERLAY:  dst[i] = blendOverlay(src[i], dst[i], opacity); break;
        }
    }
}

uint32_t LayerManager::blendNormal(uint32_t src, uint32_t dst, float opacity) {
    uint8_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    // Skip fully black source pixels (treat as transparent)
    if (sr == 0 && sg == 0 && sb == 0) return dst;
    float a = opacity;
    uint8_t rr = (uint8_t)(sr * a + dr * (1.0f - a));
    uint8_t rg = (uint8_t)(sg * a + dg * (1.0f - a));
    uint8_t rb = (uint8_t)(sb * a + db * (1.0f - a));
    return 0xFF000000 | (rr << 16) | (rg << 8) | rb;
}

uint32_t LayerManager::blendAdd(uint32_t src, uint32_t dst, float opacity) {
    uint8_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    uint8_t rr = (uint8_t)std::min(255, (int)(dr + sr * opacity));
    uint8_t rg = (uint8_t)std::min(255, (int)(dg + sg * opacity));
    uint8_t rb = (uint8_t)std::min(255, (int)(db + sb * opacity));
    return 0xFF000000 | (rr << 16) | (rg << 8) | rb;
}

uint32_t LayerManager::blendMultiply(uint32_t src, uint32_t dst, float opacity) {
    uint8_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    uint8_t mr = (uint8_t)((sr * dr) / 255);
    uint8_t mg = (uint8_t)((sg * dg) / 255);
    uint8_t mb = (uint8_t)((sb * db) / 255);
    uint8_t rr = (uint8_t)(mr * opacity + dr * (1.0f - opacity));
    uint8_t rg = (uint8_t)(mg * opacity + dg * (1.0f - opacity));
    uint8_t rb = (uint8_t)(mb * opacity + db * (1.0f - opacity));
    return 0xFF000000 | (rr << 16) | (rg << 8) | rb;
}

uint32_t LayerManager::blendScreen(uint32_t src, uint32_t dst, float opacity) {
    uint8_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    uint8_t mr = 255 - (uint8_t)(((255 - sr) * (255 - dr)) / 255);
    uint8_t mg = 255 - (uint8_t)(((255 - sg) * (255 - dg)) / 255);
    uint8_t mb = 255 - (uint8_t)(((255 - sb) * (255 - db)) / 255);
    uint8_t rr = (uint8_t)(mr * opacity + dr * (1.0f - opacity));
    uint8_t rg = (uint8_t)(mg * opacity + dg * (1.0f - opacity));
    uint8_t rb = (uint8_t)(mb * opacity + db * (1.0f - opacity));
    return 0xFF000000 | (rr << 16) | (rg << 8) | rb;
}

uint32_t LayerManager::blendOverlay(uint32_t src, uint32_t dst, float opacity) {
    uint8_t sr = (src >> 16) & 0xFF, sg = (src >> 8) & 0xFF, sb = src & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;

    auto overlayChannel = [](uint8_t s, uint8_t d) -> uint8_t {
        if (d < 128)
            return (uint8_t)std::min(255, (2 * s * d) / 255);
        else
            return (uint8_t)std::min(255, 255 - (2 * (255 - s) * (255 - d)) / 255);
    };

    uint8_t mr = overlayChannel(sr, dr);
    uint8_t mg = overlayChannel(sg, dg);
    uint8_t mb = overlayChannel(sb, db);
    uint8_t rr = (uint8_t)(mr * opacity + dr * (1.0f - opacity));
    uint8_t rg = (uint8_t)(mg * opacity + dg * (1.0f - opacity));
    uint8_t rb = (uint8_t)(mb * opacity + db * (1.0f - opacity));
    return 0xFF000000 | (rr << 16) | (rg << 8) | rb;
}

const char* LayerManager::blendModeName(BlendMode mode) {
    switch (mode) {
        case BlendMode::NORMAL:   return "N";
        case BlendMode::ADD:      return "A";
        case BlendMode::MULTIPLY: return "M";
        case BlendMode::SCREEN:   return "S";
        case BlendMode::OVERLAY:  return "O";
    }
    return "?";
}

BlendMode LayerManager::nextBlendMode(BlendMode mode) {
    int m = (int)mode;
    m = (m + 1) % 5;
    return (BlendMode)m;
}
