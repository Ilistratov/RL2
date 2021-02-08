#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::Pipeline {

struct BindingInfo {
	std::vector<vk::DescriptorBufferInfo> bufferInfo;
	std::vector<vk::DescriptorImageInfo> imageInfo;

	uint64_t bindingId;

	vk::ShaderStageFlags stage;
	vk::DescriptorType type;

	uint64_t getDescriptorCount() const;
	vk::WriteDescriptorSet getWrite() const;
	vk::DescriptorSetLayoutBinding getLayoutBinding() const;
};

}