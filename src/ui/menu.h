#pragma once
#include "../engine/renderer.h"
#include "../input.h"
#include <string>
#include <vector>

class Menu {
public:
    void open();
    void close();
    bool isOpen() const { return m_open; }

    void update(Input& input);
    void render(Renderer& r);

    int selectedIndex() const { return m_cursor; }

    // Menu items
    std::vector<std::string> items = {
        "NEW PROJECT",   // 0
        "MUSIC",         // 1
        "SAVE PRESET",   // 2
        "LOAD PRESET",   // 3
        "EXPORT .PDVIZ", // 4
        "PERFORMANCE MODE", // 5
        "RECORD",        // 6
        "SCENES",        // 7
        "SETTINGS",      // 8
        "ABOUT",         // 9
        "EXIT"           // 10
    };

private:
    bool m_open = false;
    int  m_cursor = 0;
};
