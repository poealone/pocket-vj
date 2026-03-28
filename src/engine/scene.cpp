#include "scene.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

SceneManager::SceneManager() {
    for (int i = 0; i < MAX_SCENES; i++) {
        m_scenes[i].name = "Scene " + std::to_string(i + 1);
    }
}

// ---- Minimal JSON serializer for scene state ----

std::string SceneManager::serializeState(LayerManager& layers) {
    std::string json = "{\"layers\":[";

    for (int li = 0; li < layers.layerCount(); li++) {
        const Layer& layer = layers.layer(li);
        if (li > 0) json += ",";
        json += "{\"blend\":";
        json += std::to_string((int)layer.blend);
        json += ",\"opacity\":";
        char obuf[32]; snprintf(obuf, sizeof(obuf), "%.4f", layer.opacity);
        json += obuf;
        json += ",\"mute\":";
        json += layer.mute ? "1" : "0";
        json += ",\"nodes\":[";

        for (int ni = 0; ni < (int)layer.nodes.size(); ni++) {
            VisualNode* node = layer.nodes[ni];
            if (!node) continue;
            node->syncParams();

            if (ni > 0) json += ",";
            json += "{\"type\":\"";
            json += node->typeName();
            json += "\",\"active\":";
            json += node->active ? "1" : "0";
            json += ",\"params\":{";

            const auto& plist = node->params.params();
            for (int pi = 0; pi < (int)plist.size(); pi++) {
                if (pi > 0) json += ",";
                json += "\"";
                json += plist[pi].name;
                json += "\":";
                char pbuf[32]; snprintf(pbuf, sizeof(pbuf), "%.4f", plist[pi].value);
                json += pbuf;
            }
            json += "}}";
        }
        json += "]}";
    }
    json += "]}";
    return json;
}

void SceneManager::saveScene(int slot, LayerManager& layers) {
    if (slot < 0 || slot >= MAX_SCENES) return;
    m_scenes[slot].serializedData = serializeState(layers);
    m_scenes[slot].active = true;
    m_current = slot;
}

void SceneManager::loadScene(int slot, LayerManager& layers) {
    if (slot < 0 || slot >= MAX_SCENES) return;
    if (!m_scenes[slot].active) return;

    // Parse and apply the snapshot
    SceneSnapshot snap = parseSnapshot(m_scenes[slot].serializedData);
    auto allNodes = layers.allNodes();

    // Apply params to matching nodes (by index, since structure should match)
    for (int i = 0; i < (int)snap.nodes.size() && i < (int)allNodes.size(); i++) {
        for (auto& ps : snap.nodes[i].params) {
            allNodes[i]->setParam(ps.paramName, ps.value);
        }
    }

    m_current = slot;
}

void SceneManager::crossfade(int fromSlot, int toSlot, float progress, LayerManager& layers) {
    if (!m_scenes[fromSlot].active || !m_scenes[toSlot].active) return;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    SceneSnapshot snapFrom = parseSnapshot(m_scenes[fromSlot].serializedData);
    SceneSnapshot snapTo = parseSnapshot(m_scenes[toSlot].serializedData);
    auto allNodes = layers.allNodes();

    int nodeCount = (int)allNodes.size();
    int fromCount = (int)snapFrom.nodes.size();
    int toCount = (int)snapTo.nodes.size();

    for (int i = 0; i < nodeCount; i++) {
        if (i >= fromCount || i >= toCount) break;

        auto& fromNode = snapFrom.nodes[i];
        auto& toNode = snapTo.nodes[i];

        // Build a map from param name → value for the "to" snapshot
        for (auto& fp : fromNode.params) {
            float fromVal = fp.value;
            float toVal = fromVal;  // default: keep from value

            // Find matching param in toNode
            for (auto& tp : toNode.params) {
                if (tp.paramName == fp.paramName) {
                    toVal = tp.value;
                    break;
                }
            }

            // Lerp
            float val = fromVal + (toVal - fromVal) * progress;
            allNodes[i]->setParam(fp.paramName, val);
        }
    }
}

// ---- Snapshot helpers ----

SceneSnapshot SceneManager::takeSnapshot(LayerManager& layers) {
    SceneSnapshot snap;
    auto allNodes = layers.allNodes();
    for (auto* node : allNodes) {
        NodeSnapshot ns;
        ns.typeName = node->typeName();
        node->syncParams();
        for (auto& p : node->params.params()) {
            ns.params.push_back({p.name, p.value});
        }
        snap.nodes.push_back(ns);
    }
    return snap;
}

// Minimal parser for our JSON format
SceneSnapshot SceneManager::parseSnapshot(const std::string& data) {
    SceneSnapshot snap;
    if (data.empty()) return snap;

    const char* p = data.c_str();

    // Helper lambdas
    auto skip = [&]() { while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++; };
    auto parseStr = [&]() -> std::string {
        std::string r;
        if (*p != '"') return r;
        p++;
        while (*p && *p != '"') {
            if (*p == '\\' && *(p+1)) { p++; r += *p; }
            else r += *p;
            p++;
        }
        if (*p == '"') p++;
        return r;
    };
    auto parseNum = [&]() -> float {
        char buf[64]; int i = 0;
        if (*p == '-') buf[i++] = *p++;
        while ((*p >= '0' && *p <= '9') || *p == '.') {
            if (i < 62) buf[i++] = *p;
            p++;
        }
        buf[i] = 0;
        return (float)atof(buf);
    };

    // Find "nodes" arrays inside layers
    // We flatten all nodes across all layers
    while (*p) {
        // Look for "params" objects inside node objects
        if (*p == '"') {
            const char* save = p;
            std::string key = parseStr();
            skip();
            if (*p == ':') {
                p++; skip();
                if (key == "type") {
                    // We're inside a node object — start a new NodeSnapshot
                    std::string typeName = parseStr();
                    snap.nodes.push_back(NodeSnapshot());
                    snap.nodes.back().typeName = typeName;
                } else if (key == "params" && !snap.nodes.empty()) {
                    // Parse params object
                    if (*p == '{') {
                        p++; // skip {
                        while (*p) {
                            skip();
                            if (*p == '}') { p++; break; }
                            if (*p == ',') { p++; continue; }
                            std::string pname = parseStr();
                            skip();
                            if (*p == ':') p++;
                            skip();
                            float val = parseNum();
                            snap.nodes.back().params.push_back({pname, val});
                        }
                    }
                } else if (key == "active" || key == "blend" || key == "opacity" || key == "mute") {
                    // Skip simple values
                    if (*p == '"') parseStr();
                    else parseNum();
                } else {
                    // Skip value
                    if (*p == '"') parseStr();
                    else if (*p == '{') {
                        int d = 1; p++;
                        while (*p && d > 0) { if (*p == '{') d++; else if (*p == '}') d--; p++; }
                    } else if (*p == '[') {
                        int d = 1; p++;
                        while (*p && d > 0) { if (*p == '[') d++; else if (*p == ']') d--; p++; }
                    } else parseNum();
                }
            } else {
                p = save + 1; // Not a key:value, advance past quote
            }
        } else {
            p++;
        }
    }

    return snap;
}
