#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DataComponent {

struct CommandPoolData {
	vk::CommandPool pool;
	std::vector<vk::CommandBuffer> cmd;
};

}