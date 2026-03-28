#pragma once
#include "../engine/renderer.h"
#include "../engine/visual_node.h"
#include "../input.h"
#include <string>
#include <vector>

// Entry in the node browser
struct NodeBrowserEntry {
    std::string name;
    std::string description;
    NodeCategory category;
    int nodeTypeId;  // Used to create the node
};

class NodeBrowser {
public:
    NodeBrowser();

    void open();
    void close();
    bool isOpen() const { return m_open; }

    // Returns -1 if no selection, or nodeTypeId if user picked one
    int update(Input& input);

    void render(Renderer& r);

    // Was the browser cancelled?
    bool cancelled() const { return m_cancelled; }

private:
    bool m_open = false;
    bool m_cancelled = false;
    int  m_cursor = 0;
    int  m_scrollOffset = 0;
    static const int VISIBLE_ROWS = 14;

    std::vector<NodeBrowserEntry> m_entries;
};
