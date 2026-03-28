#pragma once
#include "../engine/layer.h"
#include "../engine/visual_node.h"
#include <string>

namespace PdViz {

// Export current visual state as a .pdviz JSON file for Pocket DAW integration
bool exportFile(const std::string& path, const std::string& name, LayerManager& layers);

// Map visual node type to pdviz type string
const char* nodeTypeToPdviz(const char* typeName);

} // namespace PdViz
