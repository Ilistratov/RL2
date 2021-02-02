#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Utill\UtillFunctions.h"
#include "Renderer\ResourceHandler\StructuredBuffer.h"

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
	
	template<typename T>
	BindingInfo(const T& t, vk::ShaderStageFlags stage);
};

template<typename T>
inline BindingInfo::BindingInfo(const T& t, vk::ShaderStageFlags stage) {
	throw std::runtime_error(gen_err_str(__FILE__, __LINE__,
		"No known way to create bindingInfo for " + typeid(T).name())
	);
}

template<>
inline BindingInfo::BindingInfo(
	const ResourceHandler::StructuredBuffer& buff,
	vk::ShaderStageFlags stage
) : stage(stage) {
	bufferInfo = {
		vk::DescriptorBufferInfo{
			buff.getBufferData().buff,
			0,
			VK_WHOLE_SIZE
		}
	};

	type = vk::DescriptorType::eStorageBuffer;
}

class DSetLayoutFactory {
	std::vector<BindingInfo> bindings;

public:
	DSetLayoutFactory(uint64_t reservedSize = 0);
	
	template<typename T>
	void addBinding(const T& t, vk::ShaderStageFlags visibleAt);

	std::vector<vk::DescriptorSetLayoutBinding>&& genLayoutBindings() const;
	std::vector<vk::WriteDescriptorSet>&& genDescriptorWrites(vk::DescriptorSet dSet) const;
	std::unordered_map<vk::DescriptorType, uint64_t>&& genPoolSizes() const;
};

template<typename T>
inline void DSetLayoutFactory::addBinding(const T& t, vk::ShaderStageFlags visibleAt) {
	bindings.push_back(BindingInfo<T>(t, visibleAt));
	bindings.back().bindingId = bindings.size() - 1;
}

}
