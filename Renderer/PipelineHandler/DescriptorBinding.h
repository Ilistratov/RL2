#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::Pipeline {

class DescriptorBinding {
	std::vector<vk::DescriptorBufferInfo> buff;
	std::vector<vk::DescriptorImageInfo> img;

	uint32_t bindingId = UINT32_MAX;

	vk::ShaderStageFlags stage;
	vk::DescriptorType type;
public:
	DescriptorBinding() = default;

	DescriptorBinding(
		std::vector<vk::DescriptorBufferInfo> buff,
		std::vector<vk::DescriptorImageInfo> img,
		vk::ShaderStageFlags stage,
		vk::DescriptorType type
	);

	void setBindingId(uint32_t nId);

	uint32_t getCount() const;
	vk::WriteDescriptorSet getWrite() const;
	vk::DescriptorSetLayoutBinding getLayoutBinding() const;
};

//represents object that
//can be bound to a single DescriptorBinding
//within a descriptor set
class IDescriptorBindable {
public:
	virtual DescriptorBinding getBinding() const = 0;
};

//repreents object that can be
//bound to one descriptor set
//consists of multible bindings which can be obtained
//via getBindings()
class SetBindable {
	std::vector<DescriptorBinding> bindings;
public:
	SetBindable(const std::vector<IDescriptorBindable*>& bnd);
	const std::vector<DescriptorBinding>& getBindings() const;
};

}