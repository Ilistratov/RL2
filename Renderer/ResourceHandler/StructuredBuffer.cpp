#include "StructuredBuffer.h"

namespace Renderer::ResourceHandler {

vk::BufferCopy StructuredBufferCopy::operator()(uint64_t stride) const {
	return vk::BufferCopy(srcOffset * stride, dstOffset * stride, count * stride);
}

StructuredBuffer::StructuredBuffer(
	uint64_t count,
	uint64_t stride,
	vk::BufferUsageFlagBits specificUsage,
	uint64_t reservedBufferDataId) : count(count), stride(stride) {
	BufferBase::BufferBase(
		stride * count,
		vk::BufferUsageFlagBits::eStorageBuffer |
		vk::BufferUsageFlagBits::eTransferDst |
		specificUsage,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		reservedBufferDataId
	);
}

void StructuredBuffer::recordCopyFrom(
	vk::CommandBuffer cb,
	const DataComponent::BufferData& src,
	const std::vector<StructuredBufferCopy>& copyRegions
) {
	if (copyRegions.empty()) {
		recordCopyFrom(cb, src, { {0, 0, count} });
	}

	std::vector<vk::BufferCopy> cpyReg(copyRegions.size());

	for (uint64_t i = 0; i < copyRegions.size(); i++) {
		cpyReg[i] = copyRegions[i](stride);
	}

	cb.copyBuffer(src.buff, getBufferData().buff, cpyReg);
}

}