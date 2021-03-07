#include <unordered_map>

#include "Renderer\Core.h"
#include "Layout.h"

namespace Renderer::DescriptorHandler {

Layout::Layout(const std::vector<std::vector<LayoutBinding>>& bindings) {
	layt.reserve(bindings.size());
	std::unordered_map<vk::DescriptorType, uint32_t> typeCnt;

	for (const auto& setBindings : bindings) {
		std::vector<vk::DescriptorSetLayoutBinding> layoutBindingVk(setBindings.size());

		for (uint32_t i = 0; i < setBindings.size(); i++) {
			layoutBindingVk[i] = vk::DescriptorSetLayoutBinding{
				i,
				setBindings[i].type,
				setBindings[i].count,
				setBindings[i].stage
			};

			typeCnt[setBindings[i].type] += setBindings[i].count;
		}

		layt.push_back(
			core.device().createDescriptorSetLayout(
				vk::DescriptorSetLayoutCreateInfo{
					{},
					layoutBindingVk
				}
			)
		);
	}

	size.reserve(typeCnt.size());
	for (const auto& [type, cnt] : typeCnt) {
		size.push_back(vk::DescriptorPoolSize{ type, cnt });
	}
}

}