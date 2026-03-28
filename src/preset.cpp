#include "preset.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>

// ---- Minimal JSON helpers (no external lib) ----

static void writeJsonString(FILE* f, const std::string& s) {
    fputc('"', f);
    for (char c : s) {
        if (c == '"') fputs("\\\"", f);
        else if (c == '\\') fputs("\\\\", f);
        else fputc(c, f);
    }
    fputc('"', f);
}

namespace Preset {

bool save(const std::string& path,
          const std::string& name,
          float bpm,
          const std::vector<VisualNode*>& nodes) {
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return false;

    fprintf(f, "{\n");
    fprintf(f, "  \"version\": 2,\n");
    fprintf(f, "  \"name\": ");
    writeJsonString(f, name);
    fprintf(f, ",\n");
    fprintf(f, "  \"bpm\": %.1f,\n", bpm);
    fprintf(f, "  \"nodes\": [\n");

    for (int n = 0; n < (int)nodes.size(); n++) {
        VisualNode* node = nodes[n];
        if (!node) continue;

        node->syncParams();

        fprintf(f, "    {\n");
        fprintf(f, "      \"type\": ");
        writeJsonString(f, node->typeName());
        fprintf(f, ",\n");
        fprintf(f, "      \"active\": %s,\n", node->active ? "true" : "false");
        fprintf(f, "      \"params\": {\n");

        const auto& plist = node->params.params();
        for (int i = 0; i < (int)plist.size(); i++) {
            const Param& p = plist[i];
            fprintf(f, "        ");
            writeJsonString(f, p.name);
            if (p.type == ParamType::INT || p.type == ParamType::ENUM || p.type == ParamType::TOGGLE) {
                fprintf(f, ": %d", (int)p.value);
            } else {
                fprintf(f, ": %.4f", p.value);
            }

            // Write animation fields if animated
            if (p.animated) {
                fprintf(f, ",\n        ");
                writeJsonString(f, p.name + "__anim");
                fprintf(f, ": 1,\n        ");
                writeJsonString(f, p.name + "__amin");
                fprintf(f, ": %.4f,\n        ", p.animMin);
                writeJsonString(f, p.name + "__amax");
                fprintf(f, ": %.4f,\n        ", p.animMax);
                writeJsonString(f, p.name + "__aspd");
                fprintf(f, ": %.4f,\n        ", p.animSpeed);
                writeJsonString(f, p.name + "__ashp");
                fprintf(f, ": %d", p.animShape);
            }

            if (i < (int)plist.size() - 1) fprintf(f, ",");
            fprintf(f, "\n");
        }

        fprintf(f, "      }\n");
        fprintf(f, "    }");
        if (n < (int)nodes.size() - 1) fprintf(f, ",");
        fprintf(f, "\n");
    }

    fprintf(f, "  ]\n");
    fprintf(f, "}\n");

    fclose(f);
    return true;
}

// ---- Minimal JSON parser (just enough for presets) ----

static void skipWhitespace(const char*& p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
}

static std::string parseString(const char*& p) {
    std::string result;
    if (*p != '"') return result;
    p++; // skip opening quote
    while (*p && *p != '"') {
        if (*p == '\\' && *(p + 1)) {
            p++;
            if (*p == '"') result += '"';
            else if (*p == '\\') result += '\\';
            else if (*p == 'n') result += '\n';
            else result += *p;
        } else {
            result += *p;
        }
        p++;
    }
    if (*p == '"') p++; // skip closing quote
    return result;
}

static float parseNumber(const char*& p) {
    char buf[64];
    int i = 0;
    if (*p == '-') buf[i++] = *p++;
    while ((*p >= '0' && *p <= '9') || *p == '.') {
        if (i < 62) buf[i++] = *p;
        p++;
    }
    buf[i] = 0;
    return (float)atof(buf);
}

static bool parseBool(const char*& p) {
    if (strncmp(p, "true", 4) == 0) { p += 4; return true; }
    if (strncmp(p, "false", 5) == 0) { p += 5; return false; }
    return false;
}

PresetData load(const std::string& path) {
    PresetData data;

    FILE* f = fopen(path.c_str(), "r");
    if (!f) return data;

    // Read entire file
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0 || size > 1024 * 64) { fclose(f); return data; }

    std::vector<char> buf(size + 1);
    size_t readBytes = fread(buf.data(), 1, size, f);
    buf[readBytes] = 0;
    fclose(f);

    const char* p = buf.data();
    skipWhitespace(p);
    if (*p != '{') return data;
    p++;

    // Parse top-level object
    while (*p) {
        skipWhitespace(p);
        if (*p == '}') break;
        if (*p == ',') { p++; continue; }

        std::string key = parseString(p);
        skipWhitespace(p);
        if (*p == ':') p++;
        skipWhitespace(p);

        if (key == "name") {
            data.name = parseString(p);
        } else if (key == "bpm") {
            data.bpm = parseNumber(p);
        } else if (key == "version") {
            parseNumber(p); // skip
        } else if (key == "nodes") {
            // Parse array of nodes
            if (*p == '[') {
                p++;
                while (*p) {
                    skipWhitespace(p);
                    if (*p == ']') { p++; break; }
                    if (*p == ',') { p++; continue; }
                    if (*p != '{') break;
                    p++; // skip {

                    NodeData nd;
                    while (*p) {
                        skipWhitespace(p);
                        if (*p == '}') { p++; break; }
                        if (*p == ',') { p++; continue; }

                        std::string nkey = parseString(p);
                        skipWhitespace(p);
                        if (*p == ':') p++;
                        skipWhitespace(p);

                        if (nkey == "type") {
                            nd.typeName = parseString(p);
                        } else if (nkey == "active") {
                            parseBool(p); // handled separately
                        } else if (nkey == "params") {
                            // Parse params object
                            if (*p == '{') {
                                p++;
                                while (*p) {
                                    skipWhitespace(p);
                                    if (*p == '}') { p++; break; }
                                    if (*p == ',') { p++; continue; }

                                    std::string pkey = parseString(p);
                                    skipWhitespace(p);
                                    if (*p == ':') p++;
                                    skipWhitespace(p);
                                    float val = parseNumber(p);
                                    nd.params.push_back({pkey, val});
                                }
                            }
                        } else {
                            // Skip unknown value
                            if (*p == '"') parseString(p);
                            else if (*p == 't' || *p == 'f') parseBool(p);
                            else if ((*p >= '0' && *p <= '9') || *p == '-') parseNumber(p);
                        }
                    }
                    data.nodes.push_back(nd);
                }
            }
        } else {
            // Skip unknown value
            if (*p == '"') parseString(p);
            else if (*p == '[') {
                int depth = 1; p++;
                while (*p && depth > 0) {
                    if (*p == '[') depth++;
                    else if (*p == ']') depth--;
                    p++;
                }
            } else if (*p == '{') {
                int depth = 1; p++;
                while (*p && depth > 0) {
                    if (*p == '{') depth++;
                    else if (*p == '}') depth--;
                    p++;
                }
            } else if (*p == 't' || *p == 'f') parseBool(p);
            else if ((*p >= '0' && *p <= '9') || *p == '-') parseNumber(p);
        }
    }

    data.valid = true;
    return data;
}

std::vector<std::string> listPresets(const std::string& dir) {
    std::vector<std::string> result;

    DIR* d = opendir(dir.c_str());
    if (!d) return result;

    struct dirent* entry;
    while ((entry = readdir(d)) != nullptr) {
        std::string name = entry->d_name;
        // Check for .pvj extension
        if (name.size() > 4 && name.substr(name.size() - 4) == ".pvj") {
            result.push_back(name);
        }
    }
    closedir(d);

    std::sort(result.begin(), result.end());
    return result;
}

} // namespace Preset

// ---- PresetBrowser UI ----

void PresetBrowser::open(const std::string& dir, bool saving) {
    m_dir = dir;
    m_saving = saving;
    m_open = true;
    m_cancelled = false;
    m_cursor = 0;
    m_scrollOffset = 0;
    m_naming = false;
    m_saveName = "preset";
    m_files = Preset::listPresets(dir);
}

void PresetBrowser::close() {
    m_open = false;
}

std::string PresetBrowser::update(Input& input) {
    if (!m_open) return "";

    if (m_naming) {
        // Name entry mode (simplified: cycle chars)
        if (input.pressed(Button::LEFT)) {
            if (!m_saveName.empty()) m_saveName.pop_back();
        }
        if (input.pressed(Button::RIGHT)) {
            // Add next char (cycle a-z, 0-9)
            if (m_saveName.size() < 16) {
                m_saveName += 'a';
            }
        }
        if (input.pressed(Button::UP)) {
            // Change last char
            if (!m_saveName.empty()) {
                char& c = m_saveName.back();
                if (c >= 'a' && c < 'z') c++;
                else if (c == 'z') c = '0';
                else if (c >= '0' && c < '9') c++;
                else if (c == '9') c = '-';
                else if (c == '-') c = '_';
                else if (c == '_') c = 'a';
                else c = 'a';
            }
        }
        if (input.pressed(Button::DOWN)) {
            if (!m_saveName.empty()) {
                char& c = m_saveName.back();
                if (c > 'a' && c <= 'z') c--;
                else if (c == 'a') c = '_';
                else if (c == '_') c = '-';
                else if (c == '-') c = '9';
                else if (c > '0' && c <= '9') c--;
                else if (c == '0') c = 'z';
                else c = 'z';
            }
        }
        if (input.pressed(Button::A)) {
            // Confirm save
            std::string result = m_dir + "/" + m_saveName + ".pvj";
            close();
            return result;
        }
        if (input.pressed(Button::B)) {
            m_naming = false;
        }
        return "";
    }

    int count = (int)m_files.size();

    if (m_saving) {
        // In save mode, first option is "NEW PRESET"
        count += 1;
    }

    if (input.pressed(Button::UP)) {
        m_cursor--;
        if (m_cursor < 0) m_cursor = count - 1;
    }
    if (input.pressed(Button::DOWN)) {
        m_cursor++;
        if (m_cursor >= count) m_cursor = 0;
    }

    if (m_cursor < m_scrollOffset) m_scrollOffset = m_cursor;
    if (m_cursor >= m_scrollOffset + VISIBLE_ROWS) m_scrollOffset = m_cursor - VISIBLE_ROWS + 1;

    if (input.pressed(Button::A)) {
        if (m_saving && m_cursor == 0) {
            // Enter name mode
            m_naming = true;
            m_saveName = "preset";
            return "";
        }

        int fileIdx = m_saving ? (m_cursor - 1) : m_cursor;
        if (fileIdx >= 0 && fileIdx < (int)m_files.size()) {
            std::string result = m_dir + "/" + m_files[fileIdx];
            close();
            return result;
        }
    }

    if (input.pressed(Button::B)) {
        m_cancelled = true;
        close();
        return "";
    }

    return "";
}

void PresetBrowser::render(Renderer& r) {
    if (!m_open) return;

    // Background
    r.rect(0, 0, RENDER_W, RENDER_H, Palette::UI_BG, true);

    // Header
    r.rect(0, 0, RENDER_W, 9, {25, 15, 30}, true);
    r.text(4, 1, m_saving ? "SAVE PRESET" : "LOAD PRESET", Palette::RED);

    if (m_naming) {
        // Name entry screen
        r.text(4, 30, "Enter preset name:", Palette::UI_FG);

        // Show current name with cursor
        r.rect(4, 44, 200, 12, {30, 30, 40}, true);
        r.text(6, 46, m_saveName + "_", Palette::WHITE);

        r.text(4, 70, "UP/DN: change char", Palette::GRID);
        r.text(4, 82, "RIGHT: add char", Palette::GRID);
        r.text(4, 94, "LEFT: delete char", Palette::GRID);
        r.text(4, 106, "A: confirm   B: back", Palette::GRID);
        return;
    }

    int startY = 12;
    int rowH = 10;

    int count = (int)m_files.size();
    if (m_saving) count += 1;

    for (int i = 0; i < VISIBLE_ROWS && (m_scrollOffset + i) < count; i++) {
        int idx = m_scrollOffset + i;
        int py = startY + i * rowH;
        bool sel = (idx == m_cursor);

        if (sel) {
            r.rect(0, py - 1, RENDER_W, rowH, {40, 25, 35}, true);
        }

        if (m_saving && idx == 0) {
            r.text(10, py, "+ NEW PRESET", sel ? Palette::RED : Palette::YELLOW);
        } else {
            int fileIdx = m_saving ? (idx - 1) : idx;
            if (fileIdx >= 0 && fileIdx < (int)m_files.size()) {
                r.text(10, py, m_files[fileIdx], sel ? Palette::RED : Palette::UI_FG);
            }
        }
    }

    // Help bar
    r.rect(0, RENDER_H - 9, RENDER_W, 9, {10, 10, 16}, true);
    r.text(4, RENDER_H - 8, "A:SELECT  B:CANCEL  UP/DN:BROWSE", {100, 100, 110});
}
