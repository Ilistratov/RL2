#include "CommandPool.h"

namespace Renderer::ResourceHandler {

CommandPool::CommandPool(uint32_t queueInd) {
	data.pool = core.device().createCommandPool(
		vk::CommandPoolCreateInfo{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			queueInd
		}
	);
}

CommandPool::CommandPool(CommandPool&& other) {
	swap(other);
}

void CommandPool::operator =(CommandPool&& other) {
	swap(other);
	other.free();
}

void CommandPool::swap(CommandPool& other) {
	std::swap(data, other.data);
}

void CommandPool::free() {
	core.device().freeCommandBuffers(data.pool, data.cmd);
	core.device().destroyCommandPool(data.pool);

	data.cmd.clear();
	data.cmd.shrink_to_fit();

	data.pool = vk::CommandPool{};
}

void CommandPool::reserve(uint32_t count) {
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

vk::CommandBuffer CommandPool::reserveOneTimeSubmit() {
	return core.device().allocateCommandBuffers(
		vk::CommandBufferAllocateInfo{
			data.pool,
			vk::CommandBufferLevel::ePrimary, //TODO make it configurable
			1
		}
	)[0];
}

void CommandPool::freeOneTimeSubmit(std::vector<vk::CommandBuffer> cmd) {
	core.device().freeCommandBuffers(data.pool, cmd);
}

CommandPool::Data& CommandPool::getData() {
	return data;
}

CommandPool::~CommandPool() {
	free();
}

}