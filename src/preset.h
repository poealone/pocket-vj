#pragma once
#include "engine/visual_node.h"
#include "engine/renderer.h"
#include "input.h"
#include <string>
#include <vector>

// Minimal JSON writer/reader for presets (no external dependency)
namespace Preset {

// Save the current state to a .pvj file
bool save(const std::string& path,
          const std::string& name,
          float bpm,
          const std::vector<VisualNode*>& nodes);

// Load a preset, returning param data per node.
// Returns a vector of (typeName, paramMap) pairs.
struct NodeData {
    std::string typeName;
    std::vector<std::pair<std::string, float>> params;
};

struct PresetData {
    std::string name;
    float bpm = 120.0f;
    std::vector<NodeData> nodes;
    bool valid = false;
};

PresetData load(const std::string& path);

// List .pvj files in a directory
std::vector<std::string> listPresets(const std::string& dir);

} // namespace Preset

// UI for preset file browser
class PresetBrowser {
public:
    void open(const std::string& dir, bool saving);
    void close();
    bool isOpen() const { return m_open; }

    // Returns filename if user made a selection, empty if not
    std::string update(Input& input);
    void render(Renderer& r);

    bool isSaving() const { return m_saving; }
    bool cancelled() const { return m_cancelled; }

private:
    bool m_open = false;
    bool m_saving = false;
    bool m_cancelled = false;
    int  m_cursor = 0;
    int  m_scrollOffset = 0;
    std::string m_dir;
    std::vector<std::string> m_files;

    // For save mode: name entry
    bool m_naming = false;
    std::string m_saveName;

    static const int VISIBLE_ROWS = 14;
};
