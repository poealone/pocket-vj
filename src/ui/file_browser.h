#pragma once
#include "../engine/renderer.h"
#include "../input.h"
#include <string>
#include <vector>

struct FileEntry {
    std::string name;
    std::string fullPath;
    bool isDirectory;
    int size; // bytes, -1 for directories
};

class FileBrowser {
public:
    // Open browser at given directory, filtering by extension
    void open(const std::string& startDir, const std::string& extension = ".bmp");
    void close();
    bool isOpen() const { return m_open; }

    // Returns empty string if no selection yet, path if selected
    std::string update(Input& input);
    void render(Renderer& r);

    bool cancelled() const { return m_cancelled; }

private:
    bool m_open = false;
    bool m_cancelled = false;
    int m_cursor = 0;
    int m_scrollOffset = 0;
    std::string m_currentDir;
    std::string m_extension;
    std::vector<FileEntry> m_entries;
    static const int VISIBLE_ROWS = 16;

    void scanDirectory();
};
