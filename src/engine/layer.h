#pragma once
#include "visual_node.h"
#include "renderer.h"
#include "../config.h"
#include <vector>
#include <string>
#include <cstdint>

enum class BlendMode { NORMAL, ADD, MULTIPLY, SCREEN, OVERLAY };

struct Layer {
    std::vector<VisualNode*> nodes;
    BlendMode blend = BlendMode::NORMAL;
    float opacity = 1.0f;
    bool mute = false;
    bool solo = false;
    std::string name;
};

class LayerManager {
public:
    LayerManager();
    ~LayerManager();

    // Access
    Layer& layer(int index) { return m_layers[index]; }
    const Layer& layer(int index) const { return m_layers[index]; }
    int layerCount() const { return MAX_LAYERS; }

    // Current layer for editing
    int currentLayer() const { return m_current; }
    void setCurrentLayer(int idx);

    // Add node to current layer
    void addNode(VisualNode* node);
    // Remove node from its layer
    void removeNode(VisualNode* node);

    // Get all nodes across all layers (flat, for preset compat)
    std::vector<VisualNode*> allNodes() const;

    // Get nodes in current layer
    std::vector<VisualNode*>& currentNodes() { return m_layers[m_current].nodes; }

    // Update all active nodes across all layers
    void updateAll(float dt, float audioLevel);

    // Render all layers composited
    void renderAll(Renderer& r);

    // Blend mode name
    static const char* blendModeName(BlendMode mode);
    static BlendMode nextBlendMode(BlendMode mode);

private:
    Layer m_layers[MAX_LAYERS];
    int m_current = 0;

    // Temporary buffers for layer compositing
    uint32_t* m_layerBuf = nullptr;   // Single layer render target
    uint32_t* m_compositeBuf = nullptr; // Accumulated composite

    void renderLayer(Renderer& r, const Layer& layer);
    void compositeLayer(const uint32_t* src, uint32_t* dst, BlendMode mode, float opacity);

    // Per-pixel blend functions
    static uint32_t blendNormal(uint32_t src, uint32_t dst, float opacity);
    static uint32_t blendAdd(uint32_t src, uint32_t dst, float opacity);
    static uint32_t blendMultiply(uint32_t src, uint32_t dst, float opacity);
    static uint32_t blendScreen(uint32_t src, uint32_t dst, float opacity);
    static uint32_t blendOverlay(uint32_t src, uint32_t dst, float opacity);
};
