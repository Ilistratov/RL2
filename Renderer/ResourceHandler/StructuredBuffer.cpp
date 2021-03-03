#include "StructuredBuffer.h"

namespace Renderer::ResourceHandler {

vk::BufferCopy StructuredBufferCopy::operator()(uint64_t stride) const {
	return vk::BufferCopy(srcOffset * stride, dstOffset * stride, count * stride);
}

StructuredBuffer::StructuredBuffer(
	uint64_t count,
	uint64_t stride,
	vk::BufferUsageFlagBits specificUsage
) : BufferBase::BufferBase(
		stride* count,
		vk::BufferUsageFlagBits::eStorageBuffer |
		vk::BufferUsageFlagBits::eTransferDst |
		specificUsage,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	),
	count(count),
	stride(stride) {}

StructuredBuffer::StructuredBuffer(StructuredBuffer&& other) {
	swap(other);
}

void StructuredBuffer::operator=(StructuredBuffer&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

void StructuredBuffer::free() {
	BufferBase::free();
	stride = 1;
	count = 0;
}

void StructuredBuffer::recordCopyFrom(
	vk::CommandBuffer cb,
	const BufferBase& src,
	const std::vector<StructuredBufferCopy>& copyRegions
) {
	if (copyRegions.empty()) {
		recordCopyFrom(cb, src, { {0, 0, count} });
	}

	std::vector<vk::BufferCopy> cpyReg(copyRegions.size());

	for (uint64_t i = 0; i < copyRegions.size(); i++) {
		cpyReg[i] = copyRegions[i](stride);
	}

	cb.copyBuffer(src.getData().buff, data.buff, cpyReg);
}

std::vector<vk::BufferCopy>&& StructuredBuffer::toBufferCopy(const std::vector<StructuredBufferCopy>& cpyReg) {
	std::vector<vk::BufferCopy> res(cpyReg.size());
	
	for (uint64_t i = 0; i < cpyReg.size(); i++) {
		res[i] = cpyReg[i](stride);
	}
	
	return std::move(res);
}

vk::BufferMemoryBarrier StructuredBuffer::genUploadBarrier() {
	return genMemoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead);
}

void StructuredBuffer::swap(StructuredBuffer& other) {
	BufferBase::swap(other);
	std::swap(count, other.count);
	std::swap(stride, other.stride);
}

}