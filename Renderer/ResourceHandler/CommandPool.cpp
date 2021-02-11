#include "CommandPool.h"

namespace Renderer::ResourceHandler {

CommandPool::CommandPool(uint64_t queueInd, uint64_t reservedCommandPoolDataId) {
	if (reservedCommandPoolDataId == UINT64_MAX) {
		reservedCommandPoolDataId = core.getCommandPools().size();
		core.getCommandPools().push_back({});
	}

	commandPoolDataId = reservedCommandPoolDataId;

	auto& data = getData();

	data.pool = core.device().createCommandPool(
		vk::CommandPoolCreateInfo{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			queueInd
		}
	);
}

void CommandPool::reserve(uint64_t count) {
	auto& data = getData();
	if (count <= data.cmd.size()) {
		return;
	}

	count -= data.cmd.size();
	auto n_cmd = core.device().allocateCommandBuffers(
		vk::CommandBufferAllocateInfo{
			data.pool,
			vk::CommandBufferLevel::ePrimary, //TODO make it configurable
			count
		}
	);

	data.cmd.insert(data.cmd.end(), n_cmd.begin(), n_cmd.end());
}

DataComponent::CommandPoolData& CommandPool::getData() {
	return core.getCommandPools()[commandPoolDataId];
}

}