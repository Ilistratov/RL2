#include "DescriptorHandler.h"

namespace Renderer::Pipeline {

DescriptorHandler::DescriptorHandler(std::vector<DPoolLayoutFactory>& factory, uint64_t dPoolDataReservedId) {
	if (dPoolDataReservedId == UINT64_MAX) {
		dPoolDataReservedId = core.getDescriptorPools().size();
		core.getDescriptorPools().reserve(dPoolDataReservedId + factory.size());
	}

	dPoolDataId.reserve(factory.size());
	
	for (uint64_t i = dPoolDataReservedId; i < dPoolDataReservedId + factory.size(); i++) {
		int fr_i = i - dPoolDataReservedId;
		dPoolDataId.push_back(i);

		auto bindings = std::move(factory[fr_i].genLayoutBindings());
		auto poolSizes = std::move(factory[fr_i].genPoolSizes());
		
		core.getDescriptorPools()[i].layts.reserve(bindings.size());
	
		for (uint64_t j = 0; j < bindings.size(); j++) {
			core.getDescriptorPools()[i].layts[j] = core.device().createDescriptorSetLayout(
				vk::DescriptorSetLayoutCreateInfo{
					vk::DescriptorSetLayoutCreateFlags{},
					bindings
				}
			);
		}
		
		core.getDescriptorPools()[i].pool = core.device().createDescriptorPool(
			vk::DescriptorPoolCreateInfo{
				vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
				bindings.size(),
				poolSizes
			}
		);

		core.getDescriptorPools()[i].sets = core.device().allocateDescriptorSets(
			vk::DescriptorSetAllocateInfo{
				core.getDescriptorPools()[i].pool,
				core.getDescriptorPools()[i].layts
			}
		);

		auto writes = factory[fr_i].genDescriptorWrites(core.getDescriptorPools()[i].sets);
		core.device().updateDescriptorSets(writes, {});
	}
}

}
