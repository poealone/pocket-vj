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
        "NEW PROJECT",
        "SAVE PRESET",
        "LOAD PRESET",
        "EXPORT .PDVIZ",
        "PERFORMANCE MODE",
        "RECORD",
        "SCENES",
        "SETTINGS",
        "ABOUT",
        "EXIT"
    };

private:
    bool m_open = false;
    int  m_cursor = 0;
};
