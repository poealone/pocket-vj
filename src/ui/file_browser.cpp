#include "file_browser.h"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstdio>

void FileBrowser::open(const std::string& startDir, const std::string& extension) {
    m_open = true;
    m_cancelled = false;
    m_cursor = 0;
    m_scrollOffset = 0;
    m_currentDir = startDir;
    m_extension = extension;
    scanDirectory();
}

void FileBrowser::close() {
    m_open = false;
    m_entries.clear();
}

void FileBrowser::scanDirectory() {
    m_entries.clear();
    m_cursor = 0;
    m_scrollOffset = 0;

    DIR* dir = opendir(m_currentDir.c_str());
    if (!dir) return;

    // Add parent directory entry
    if (m_currentDir != "/") {
        FileEntry parent;
        parent.name = "..";
        parent.fullPath = m_currentDir + "/..";
        parent.isDirectory = true;
        parent.size = -1;
        m_entries.push_back(parent);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = m_currentDir + "/" + name;
        struct stat st;
        if (stat(fullPath.c_str(), &st) != 0) continue;

        FileEntry fe;
        fe.name = name;
        fe.fullPath = fullPath;
        fe.isDirectory = S_ISDIR(st.st_mode);
        fe.size = fe.isDirectory ? -1 : (int)st.st_size;

        // Filter: show directories and matching extension files
        if (!fe.isDirectory) {
            if (!m_extension.empty()) {
                std::string lower = name;
                for (auto& c : lower) c = tolower(c);
                std::string extLower = m_extension;
                for (auto& c : extLower) c = tolower(c);

                if (lower.size() < extLower.size() ||
                    lower.substr(lower.size() - extLower.size()) != extLower) {
                    continue;
                }
            }
        }

        m_entries.push_back(fe);
    }
    closedir(dir);

    // Sort: directories first, then alphabetical
    std::sort(m_entries.begin(), m_entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.name == "..") return true;
        if (b.name == "..") return false;
        if (a.isDirectory != b.isDirectory) return a.isDirectory;
        return a.name < b.name;
    });
}

std::string FileBrowser::update(Input& input) {
    if (!m_open) return "";

    int count = (int)m_entries.size();
    if (count == 0) {
        if (input.pressed(Button::B)) {
            m_cancelled = true;
            close();
        }
        return "";
    }

    if (input.pressed(Button::UP)) {
        m_cursor--;
        if (m_cursor < 0) m_cursor = count - 1;
    }
    if (input.pressed(Button::DOWN)) {
        m_cursor++;
        if (m_cursor >= count) m_cursor = 0;
    }

    // Scroll
    if (m_cursor < m_scrollOffset) m_scrollOffset = m_cursor;
    if (m_cursor >= m_scrollOffset + VISIBLE_ROWS) m_scrollOffset = m_cursor - VISIBLE_ROWS + 1;

    // Select
    if (input.pressed(Button::A) && m_cursor < count) {
        const FileEntry& sel = m_entries[m_cursor];
        if (sel.isDirectory) {
            if (sel.name == "..") {
                // Go up
                size_t slash = m_currentDir.rfind('/');
                if (slash != std::string::npos && slash > 0) {
                    m_currentDir = m_currentDir.substr(0, slash);
                } else {
                    m_currentDir = "/";
                }
            } else {
                m_currentDir = sel.fullPath;
            }
            scanDirectory();
            return "";
        } else {
            // File selected
            std::string path = sel.fullPath;
            close();
            return path;
        }
    }

    // Cancel
    if (input.pressed(Button::B)) {
        // Try going up first
        if (m_currentDir != "/" && m_currentDir.find('/') != std::string::npos) {
            size_t slash = m_currentDir.rfind('/');
            if (slash > 0) {
                m_currentDir = m_currentDir.substr(0, slash);
                scanDirectory();
                return "";
            }
        }
        m_cancelled = true;
        close();
    }

    return "";
}

void FileBrowser::render(Renderer& r) {
    if (!m_open) return;

    int count = (int)m_entries.size();

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, "FILE BROWSER", Palette::RED);

    // Current directory
    std::string dirDisplay = m_currentDir;
    if (dirDisplay.size() > 40) {
        dirDisplay = "..." + dirDisplay.substr(dirDisplay.size() - 37);
    }
    r.text(4, 11, dirDisplay, Palette::CYAN);

    if (count == 0) {
        r.text(4, 30, "(empty)", {100, 100, 100});
        r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
        r.text(4, RENDER_H - 8, "B:BACK", {100, 100, 110});
        return;
    }

    // File list
    int startY = 20;
    int rowH = 10;
    int visIdx = 0;
    int drawY = startY;

    for (int i = m_scrollOffset; i < count && visIdx < VISIBLE_ROWS; i++) {
        const auto& entry = m_entries[i];
        bool sel = (i == m_cursor);

        if (sel) {
            r.rect(0, drawY - 1, RENDER_W, rowH, {40, 25, 35}, true);
        }

        // Icon
        if (entry.isDirectory) {
            r.text(4, drawY, "[D]", Palette::YELLOW);
        } else {
            r.text(4, drawY, " . ", {80, 80, 100});
        }

        // Name (truncated)
        std::string name = entry.name;
        if (name.size() > 30) name = name.substr(0, 27) + "...";
        r.text(24, drawY, name, sel ? Palette::RED : Palette::WHITE);

        // Size (for files)
        if (!entry.isDirectory && entry.size >= 0) {
            char sizeBuf[16];
            if (entry.size < 1024) {
                snprintf(sizeBuf, sizeof(sizeBuf), "%dB", entry.size);
            } else {
                snprintf(sizeBuf, sizeof(sizeBuf), "%dK", entry.size / 1024);
            }
            r.text(RENDER_W - 30, drawY, sizeBuf, {80, 80, 100});
        }

        drawY += rowH;
        visIdx++;
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "A:SELECT  B:BACK  UP/DN:BROWSE", {100, 100, 110});
}
