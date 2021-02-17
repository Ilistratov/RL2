#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::Pipeline {

class DescriptorBinding {
	std::vector<vk::DescriptorBufferInfo> buff;
	std::vector<vk::DescriptorImageInfo> img;

	uint32_t bindingId = UINT64_MAX;

	vk::ShaderStageFlags stage;
	vk::DescriptorType type;
public:
	DescriptorBinding() = default;

	DescriptorBinding(
		std::vector<vk::DescriptorBufferInfo> buff,
		std::vector<vk::DescriptorImageInfo> img,
		uint32_t bindingId,
		vk::ShaderStageFlags stage,
		vk::DescriptorType type
	);

	uint32_t getCount() const;
	vk::WriteDescriptorSet getWrite() const;
	vk::DescriptorSetLayoutBinding getLayoutBinding() const;
};

//repreents object that can be
//bound to one descriptor set
//consists of multible bindings which can be obtained
//via getBindings()
class IDescriptorBindable {
public:
	virtual std::vector<DescriptorBinding> getBindings() const;
};

}