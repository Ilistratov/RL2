#pragma once

#include <vector>


#include "Utill\UtillFunctions.h"
#include "Renderer\ResourceHandler\StructuredBuffer.h"
#include "BindingInfo.h"

namespace Renderer::Pipeline {

class DSetLayoutFactory {
	std::vector<BindingInfo> bindings;

public:
	DSetLayoutFactory(uint64_t reservedSize = 0);
	
	template<typename T>
	inline void addBinding(const T& t, vk::ShaderStageFlags visibleAt) {
		bindings.push_back(t.genBindingInfo());
		bindings.back().bindingId = bindings.size() - 1;
		bindings.back().stage = visibleAt;
	}

	std::vector<vk::DescriptorSetLayoutBinding>&& genLayoutBindings() const;
	std::vector<vk::WriteDescriptorSet>&& genDescriptorWrites(vk::DescriptorSet dSet) const;
	std::unordered_map<vk::DescriptorType, uint64_t>&& genPoolSizes() const;
};

}
