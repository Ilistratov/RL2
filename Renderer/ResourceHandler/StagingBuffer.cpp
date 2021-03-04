#include "StagingBuffer.h"
#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

StagingBuffer::StagingBuffer(uint64_t sz) : 
	BufferBase(
		sz,
		vk::BufferUsageFlagBits::eTransferSrc |
		vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eHostVisible |
		vk::MemoryPropertyFlagBits::eHostCoherent
	) {}

void StagingBuffer::copyFrom(void* src, uint64_t sz, uint64_t dstOffset) {
	if (sz == 0) {
		sz = data.sz;
	}

	void* dst = core.device().mapMemory(data.mem, dstOffset, sz);
	std::memcpy(dst, src, sz);
	core.device().unmapMemory(data.mem);
}

void StagingBuffer::recordCopyTo(vk::CommandBuffer& cmd, BufferBase& dst, const std::vector<vk::BufferCopy>& copyRegions) {
	if (copyRegions.empty()) {
		cmd.copyBuffer(data.buff, dst.getData().buff, { {0, 0, std::min(data.sz, dst.getData().sz)} });
		return;
	}
	
	cmd.copyBuffer(data.buff, dst.getData().buff, copyRegions);
}

}