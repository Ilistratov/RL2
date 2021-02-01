#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DataComponent {

struct DescriptorPoolData {
	vk::DescriptorPool pool;
	std::vector<vk::DescriptorSet> sets;
	std::vector<vk::DescriptorSetLayout> layts;
};

}