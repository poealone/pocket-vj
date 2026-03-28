#pragma once
#include "layer.h"
#include "visual_node.h"
#include "../config.h"
#include <string>
#include <vector>

static constexpr int MAX_SCENES = 8;

struct Scene {
    std::string name;
    std::string serializedData;  // JSON string of the full state
    bool active = false;
};

// Snapshot of a single param for crossfade interpolation
struct ParamSnapshot {
    std::string paramName;
    float value;
};

// Snapshot of a single node's params
struct NodeSnapshot {
    std::string typeName;
    std::vector<ParamSnapshot> params;
};

// Snapshot of the entire layer state
struct SceneSnapshot {
    std::vector<NodeSnapshot> nodes;  // Flat list of all node params
};

class SceneManager {
public:
    SceneManager();

    // Save current LayerManager state to a scene slot
    void saveScene(int slot, LayerManager& layers);

    // Load a scene slot and restore to LayerManager
    void loadScene(int slot, LayerManager& layers);

    // Crossfade between two scenes by interpolating params
    // progress: 0.0 = fully fromSlot, 1.0 = fully toSlot
    void crossfade(int fromSlot, int toSlot, float progress, LayerManager& layers);

    // Scene access
    Scene& scene(int slot) { return m_scenes[slot]; }
    const Scene& scene(int slot) const { return m_scenes[slot]; }
    bool hasScene(int slot) const { return m_scenes[slot].active; }

    int currentScene() const { return m_current; }
    void setCurrentScene(int s) { if (s >= 0 && s < MAX_SCENES) m_current = s; }

private:
    Scene m_scenes[MAX_SCENES];
    int m_current = 0;

    // Take a snapshot of all node params for interpolation
    SceneSnapshot takeSnapshot(LayerManager& layers);

    // Parse serialized data back into a snapshot
    SceneSnapshot parseSnapshot(const std::string& data);

    // Serialize current state to JSON string
    std::string serializeState(LayerManager& layers);
};
