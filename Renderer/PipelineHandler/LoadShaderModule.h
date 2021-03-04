#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::Pipeline {

vk::UniqueShaderModule loadShader(const std::string& filePath);

}

