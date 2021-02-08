#include "DPoolHandler.h"

namespace Renderer::Pipeline {

DPoolHandler::DPoolHandler(std::vector<DSetLayoutFactory>& factory, uint64_t dPoolDataReservedId) {
	if (dPoolDataReservedId == UINT64_MAX) {
		dPoolDataReservedId = core.getDescriptorPools().size();
		core.getDescriptorPools().push_back({});
	}

	auto& data = getData();
	std::unordered_map<vk::DescriptorType, uint64_t> type_counts;

	data.layts.reserve(factory.size());

	for (uint64_t d_set_ind = 0; d_set_ind < factory.size(); d_set_ind++) {
		auto bindings(std::move(factory[d_set_ind].genLayoutBindings()));
		
		data.layts[d_set_ind] = core.device().createDescriptorSetLayout(
			vk::DescriptorSetLayoutCreateInfo{
				vk::DescriptorSetLayoutCreateFlags{},
				bindings
			}
		);

		auto d_set_type_counts(std::move(factory[d_set_ind].genPoolSizes()));

		if (d_set_type_counts.size() > type_counts.size()) {
			type_counts.swap(d_set_type_counts);
		}

		for (auto& [type, cnt] : d_set_type_counts) {
			type_counts[type] += cnt;
		}
	}
	
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	pool_sizes.reserve(type_counts.size());

	for (const auto& [type, cnt] : type_counts) {
		pool_sizes.push_back({ type, (uint32_t)cnt });
	}

	core.device().createDescriptorPool(
		vk::DescriptorPoolCreateInfo{
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			(uint32_t)factory.size(),
			pool_sizes
		}
	);

	data.sets = core.device().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo{
			data.pool,
			data.layts
		}
	);

	std::vector<vk::WriteDescriptorSet> writes;

	for (uint64_t d_set_ind = 0; d_set_ind < factory.size(); d_set_ind++) {
		auto d_set_writes(std::move(factory[d_set_ind].genDescriptorWrites(data.sets[d_set_ind])));
		writes.insert(writes.begin(), d_set_writes.begin(), d_set_writes.end());
	}

	core.device().updateDescriptorSets(writes, {});
}

DataComponent::DescriptorPoolData& DPoolHandler::getData() {
	return core.getDescriptorPools()[dPoolDataId];
}

const DataComponent::DescriptorPoolData& DPoolHandler::getData() const {
	return core.getDescriptorPools()[dPoolDataId];
}

}
