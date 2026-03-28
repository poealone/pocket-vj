#include "pdviz.h"
#include <cstdio>

namespace PdViz {

const char* nodeTypeToPdviz(const char* typeName) {
    // Map Pocket VJ node types to pdviz types
    if (!typeName) return "generic";

    // Exact 4-char type codes from our nodes
    std::string t(typeName);
    if (t == "BARS") return "spectrum";
    if (t == "WAVE") return "waveform";
    if (t == "SHPE") return "shape";
    if (t == "PTCL") return "particles";
    if (t == "CLRF") return "color_field";
    if (t == "NOIZ") return "noise";
    if (t == "LASR") return "laser";
    if (t == "STRB") return "strobe";
    if (t == "GRID") return "grid";
    if (t == "TUNL") return "tunnel";
    if (t == "STAR") return "starfield";
    if (t == "PLSM") return "plasma";
    if (t == "MIRR") return "mirror";
    if (t == "BLUR") return "blur";
    if (t == "FDBK") return "feedback";
    if (t == "GLTC") return "glitch";
    if (t == "SCAN") return "scanline";
    if (t == "PIXL") return "pixelate";
    if (t == "CSHF") return "colorshift";
    if (t == "EDGE") return "edge";
    if (t == "LFO")  return "lfo";
    if (t == "ENV")  return "envelope";
    if (t == "AMOD") return "audio_mod";
    return "generic";
}

static void writeJsonStr(FILE* f, const char* s) {
    fputc('"', f);
    while (*s) {
        if (*s == '"') fputs("\\\"", f);
        else if (*s == '\\') fputs("\\\\", f);
        else fputc(*s, f);
        s++;
    }
    fputc('"', f);
}

bool exportFile(const std::string& path, const std::string& name, LayerManager& layers) {
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return false;

    // Find the first active source node to use as the primary visual type
    VisualNode* primaryNode = nullptr;
    BlendMode primaryBlend = BlendMode::NORMAL;
    std::vector<const char*> fxList;

    for (int li = 0; li < layers.layerCount(); li++) {
        const Layer& layer = layers.layer(li);
        if (layer.mute) continue;

        for (auto* node : layer.nodes) {
            if (!node || !node->active) continue;
            node->syncParams();

            if (node->category() == NodeCategory::FX) {
                fxList.push_back(nodeTypeToPdviz(node->typeName()));
            } else if (!primaryNode && node->category() == NodeCategory::SOURCE) {
                primaryNode = node;
                primaryBlend = layer.blend;
            }
        }
    }

    if (!primaryNode) {
        // Use first node of any kind
        auto all = layers.allNodes();
        if (!all.empty()) primaryNode = all[0];
    }

    // Write JSON
    fprintf(f, "{\n");
    fprintf(f, "  \"version\": 1,\n");
    fprintf(f, "  \"name\": ");
    writeJsonStr(f, name.c_str());
    fprintf(f, ",\n");

    if (primaryNode) {
        primaryNode->syncParams();
        fprintf(f, "  \"type\": \"%s\",\n", nodeTypeToPdviz(primaryNode->typeName()));
        fprintf(f, "  \"x\": %d, \"y\": %d, \"w\": %d, \"h\": %d,\n",
                primaryNode->x, primaryNode->y, primaryNode->w, primaryNode->h);
        fprintf(f, "  \"color\": [%d, %d, %d],\n",
                primaryNode->color.r, primaryNode->color.g, primaryNode->color.b);
        fprintf(f, "  \"reactive\": %s,\n", primaryNode->reactive ? "true" : "false");

        // Type-specific params
        const Param* barsParam = primaryNode->params.find("num_bars");
        if (barsParam) {
            fprintf(f, "  \"bars\": %d,\n", (int)barsParam->value);
        }
    } else {
        fprintf(f, "  \"type\": \"generic\",\n");
        fprintf(f, "  \"x\": 0, \"y\": 0, \"w\": %d, \"h\": %d,\n", RENDER_W, RENDER_H);
        fprintf(f, "  \"color\": [255, 255, 255],\n");
        fprintf(f, "  \"reactive\": false,\n");
    }

    // FX list
    fprintf(f, "  \"fx\": [");
    for (int i = 0; i < (int)fxList.size(); i++) {
        if (i > 0) fprintf(f, ", ");
        fprintf(f, "\"%s\"", fxList[i]);
    }
    fprintf(f, "],\n");

    // Blend mode
    const char* blendStr = "normal";
    switch (primaryBlend) {
        case BlendMode::ADD:      blendStr = "add"; break;
        case BlendMode::MULTIPLY: blendStr = "multiply"; break;
        case BlendMode::SCREEN:   blendStr = "screen"; break;
        case BlendMode::OVERLAY:  blendStr = "overlay"; break;
        default: break;
    }
    fprintf(f, "  \"layer_blend\": \"%s\"\n", blendStr);

    fprintf(f, "}\n");
    fclose(f);
    return true;
}

} // namespace PdViz
