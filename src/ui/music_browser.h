#pragma once
#include "../engine/renderer.h"
#include "../input.h"
#include <string>
#include <vector>

class MusicBrowser {
public:
    void open(const std::string& startDir = "/mnt/mmc");
    void close();
    bool isOpen()     const { return m_open;      }
    bool cancelled()  const { return m_cancelled;  }

    // Returns selected file path, or empty string while browsing
    std::string update(Input& input);
    void        render(Renderer& r);

private:
    bool m_open      = false;
    bool m_cancelled = false;
    int  m_cursor     = 0;
    int  m_scrollOffset = 0;
    std::string m_currentDir;
    std::vector<std::string> m_entries;  // dirs end with '/', files don't

    void scanDir(const std::string& dir);
    bool isAudioFile(const std::string& name) const;

    static const int VISIBLE_ROWS = 16;
};
