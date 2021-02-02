#pragma once

#include "Renderer\Core.h"

namespace Renderer::Pipeline {

vk::UniqueShaderModule loadShader(const std::string& filePath);

}

