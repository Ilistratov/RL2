#include <algorithm>
#include <cassert>
#include "DPoolHandler.h"

namespace Renderer::Pipeline {

DPoolHandler::DPoolHandler(
	const std::vector<IDescriptorBindable*>& setBindables
) {
	std::vector<std::vector<DescriptorBinding>> setBindings;
	setBindings.reserve(setBindables.size());

	for (const auto& bindable : setBindables) {
		setBindings.push_back(bindable->getBindings());
	}

	data.layts.reserve(setBindings.size());
	std::unordered_map<vk::DescriptorType, uint64_t> type_counts;

	for (const auto& setBnd : setBindings) {
		std::vector<vk::DescriptorSetLayoutBinding> bndInfo;
		bndInfo.reserve(setBnd.size());

		for (const auto& bnd : setBnd) {
			bndInfo.push_back(bnd.getLayoutBinding());
			type_counts[bndInfo.back().descriptorType] += bnd.getCount();
		}

		data.layts.push_back(
			core.device().createDescriptorSetLayout(
				vk::DescriptorSetLayoutCreateInfo{
					vk::DescriptorSetLayoutCreateFlags{},
					bndInfo
				}
			)
		);
	}

	std::vector<vk::DescriptorPoolSize> pool_sizes;
	pool_sizes.reserve(type_counts.size());

	for (const auto& [type, cnt] : type_counts) {
		pool_sizes.push_back({ type, (uint32_t)cnt });
	}

	data.pool = core.device().createDescriptorPool(
		vk::DescriptorPoolCreateInfo{
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			(uint32_t)setBindings.size(),
			pool_sizes
		}
	);

	data.sets = core.device().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo{
			data.pool,
			data.layts
		}
	);

	for (uint32_t setInd = 0; setInd < setBindings.size(); setInd++) {
		std::vector<vk::WriteDescriptorSet> writes;
		writes.reserve(setBindings[setInd].size());
		
		for (const auto& bnd : setBindings[setInd]) {
			writes.push_back(bnd.getWrite());
			writes.back().dstSet = data.sets[setInd];
		}

		core.device().updateDescriptorSets(writes, {});
	}
}

DPoolHandler::Data& DPoolHandler::getData() {
	return data;
}

const DPoolHandler::Data& DPoolHandler::getData() const {
	return data;
}

DPoolHandler::~DPoolHandler() {
	free();
}

DPoolHandler::DPoolHandler(DPoolHandler&& other) {
	swap(other);
}

void DPoolHandler::operator=(DPoolHandler&& other) {
	swap(other);
	other.free();
}

void DPoolHandler::swap(DPoolHandler& other) {
	if (this == &other) {
		return;
	}

	std::swap(data, other.data);
}

void DPoolHandler::free() {
	for (auto layt : data.layts) {
		core.device().destroyDescriptorSetLayout(layt);
	}
	
	core.device().destroyDescriptorPool(data.pool);

	data.layts.clear();
	data.layts.shrink_to_fit();
	
	data.sets.clear();
	data.sets.shrink_to_fit();

	data.pool = vk::DescriptorPool{};
}

}
