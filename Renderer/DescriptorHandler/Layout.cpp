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

void Layout::swap(Layout& other) {
	layt.swap(other.layt);
	size.swap(other.size);
}

void Layout::free() {
	for (auto& l : layt) {
		core.device().destroyDescriptorSetLayout(l);
	}

	layt.clear();
	layt.shrink_to_fit();

	size.clear();
	size.shrink_to_fit();
}

Layout::Layout(Layout&& other) {
	swap(other);
}

void Layout::operator=(Layout&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

Layout::~Layout() {
	free();
}

const std::vector<vk::DescriptorSetLayout>& Layout::getLayouts() const {
	return layt;
}

const std::vector<vk::DescriptorPoolSize>& Layout::getSizes() const {
	return size;
}

uint32_t Layout::getSetCount() const {
	return layt.size();
}

}