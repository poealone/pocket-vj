#include "music_browser.h"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cctype>
#include <cstdio>

// ---- helpers ---------------------------------------------------------------

static std::string strToLower(const std::string& s) {
    std::string r = s;
    for (auto& c : r) c = (char)tolower((unsigned char)c);
    return r;
}

bool MusicBrowser::isAudioFile(const std::string& name) const {
    std::string lo = strToLower(name);
    if (lo.size() >= 5 && lo.substr(lo.size() - 5) == ".flac") return true;
    if (lo.size() >= 4) {
        std::string ext4 = lo.substr(lo.size() - 4);
        if (ext4 == ".ogg" || ext4 == ".mp3" || ext4 == ".wav") return true;
    }
    return false;
}

// ---- scan ------------------------------------------------------------------

void MusicBrowser::scanDir(const std::string& dir) {
    m_entries.clear();
    m_cursor       = 0;
    m_scrollOffset = 0;

    // Always add parent entry unless we're literally at "/"
    if (dir != "/") {
        m_entries.push_back("../");
    }

    DIR* d = opendir(dir.c_str());
    if (!d) return;

    std::vector<std::string> dirs;
    std::vector<std::string> files;

    struct dirent* de;
    while ((de = readdir(d)) != nullptr) {
        std::string name = de->d_name;
        if (name == "." || name == "..") continue;

        std::string full = (dir == "/") ? "/" + name : dir + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            dirs.push_back(name + "/");
        } else if (isAudioFile(name)) {
            files.push_back(name);
        }
    }
    closedir(d);

    std::sort(dirs.begin(),  dirs.end());
    std::sort(files.begin(), files.end());

    for (auto& s : dirs)  m_entries.push_back(s);
    for (auto& s : files) m_entries.push_back(s);
}

// ---- public API ------------------------------------------------------------

void MusicBrowser::open(const std::string& startDir) {
    m_open        = true;
    m_cancelled   = false;
    m_currentDir  = startDir;
    scanDir(m_currentDir);
}

void MusicBrowser::close() {
    m_open = false;
    m_entries.clear();
}

std::string MusicBrowser::update(Input& input) {
    if (!m_open) return "";

    int count = (int)m_entries.size();

    // Navigation
    if (input.pressed(Button::UP)) {
        if (count > 0) {
            m_cursor--;
            if (m_cursor < 0) m_cursor = count - 1;
        }
    }
    if (input.pressed(Button::DOWN)) {
        if (count > 0) {
            m_cursor++;
            if (m_cursor >= count) m_cursor = 0;
        }
    }

    // Scroll
    if (count > 0) {
        if (m_cursor < m_scrollOffset) m_scrollOffset = m_cursor;
        if (m_cursor >= m_scrollOffset + VISIBLE_ROWS)
            m_scrollOffset = m_cursor - VISIBLE_ROWS + 1;
    }

    // A: enter dir or select file
    if (input.pressed(Button::A) && count > 0) {
        const std::string& sel = m_entries[m_cursor];
        bool isDir = (sel.back() == '/');

        if (isDir) {
            if (sel == "../") {
                // Navigate up
                size_t slash = m_currentDir.rfind('/');
                if (slash == 0)                         m_currentDir = "/";
                else if (slash != std::string::npos)    m_currentDir = m_currentDir.substr(0, slash);
            } else {
                // Navigate into subdir
                std::string sub = sel.substr(0, sel.size() - 1);
                if (m_currentDir == "/")  m_currentDir = "/" + sub;
                else                      m_currentDir += "/" + sub;
            }
            scanDir(m_currentDir);
            return "";
        } else {
            // Audio file selected
            std::string path = (m_currentDir == "/") ? "/" + sel : m_currentDir + "/" + sel;
            close();
            return path;
        }
    }

    // B: go up one directory, or cancel at root
    if (input.pressed(Button::B)) {
        if (m_currentDir != "/") {
            size_t slash = m_currentDir.rfind('/');
            if (slash == 0)                         m_currentDir = "/";
            else if (slash != std::string::npos)    m_currentDir = m_currentDir.substr(0, slash);
            scanDir(m_currentDir);
            return "";
        }
        // At root → cancel
        m_cancelled = true;
        close();
        return "";
    }

    // Select / Start → cancel
    if (input.pressed(Button::SELECT) || input.pressed(Button::START)) {
        m_cancelled = true;
        close();
    }

    return "";
}

void MusicBrowser::render(Renderer& r) {
    if (!m_open) return;

    int count = (int)m_entries.size();

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, "MUSIC BROWSER", Palette::RED);

    // Current directory (truncated)
    std::string dirDisplay = m_currentDir;
    if (dirDisplay.size() > 40) {
        dirDisplay = "..." + dirDisplay.substr(dirDisplay.size() - 37);
    }
    r.text(4, 11, dirDisplay, Palette::CYAN);

    if (count == 0) {
        r.text(4, 35, "(no audio files found)", {100, 100, 100});
        r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
        r.text(4, RENDER_H - 8, "B:BACK  SELECT:CANCEL", {100, 100, 110});
        return;
    }

    // Entry list
    int startY = 22;
    int rowH   = 10;

    for (int i = m_scrollOffset, drawn = 0;
         i < count && drawn < VISIBLE_ROWS;
         i++, drawn++)
    {
        const std::string& entry = m_entries[i];
        bool selected = (i == m_cursor);
        bool isDir    = (entry.back() == '/');

        if (selected) {
            r.rect(0, startY + drawn * rowH - 1, RENDER_W, rowH, {50, 20, 40}, true);
        }

        // Truncate long names
        std::string name = entry;
        if (name.size() > 38) name = name.substr(0, 35) + "...";

        Color col;
        if (selected)  col = Palette::RED;
        else if (isDir) col = Palette::CYAN;
        else            col = Palette::WHITE;

        r.text(8, startY + drawn * rowH, name, col);
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "A:SELECT/ENTER  B:UP/CANCEL  SELECT:CANCEL", {100, 100, 110});
}
