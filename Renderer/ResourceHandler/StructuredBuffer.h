#pragma once

#include <vector>

#include "Renderer\DescriptorHandler\Pool.h"
#include "BufferBase.h"

namespace Renderer::ResourceHandler {

struct StructuredBufferCopy {
	uint64_t srcOffset = 0, dstOffset = 0, count = 0;
	vk::BufferCopy operator()(uint64_t stride) const;
};

class StructuredBuffer : public BufferBase {
	uint64_t count = 0, stride = 1;
public:
	StructuredBuffer(const StructuredBuffer&) = delete;
	StructuredBuffer& operator =(const StructuredBuffer&) = delete;

	StructuredBuffer() = default;

	StructuredBuffer(
		uint64_t count,
		uint64_t stride = 1,
		vk::BufferUsageFlagBits specificUsage = {}
	);

	void swap(StructuredBuffer& other);
	void free();

	StructuredBuffer(StructuredBuffer&& other);
	void operator =(StructuredBuffer&& other);

	void recordCopyFrom(
		vk::CommandBuffer cb,
		const BufferBase& src,
		const std::vector<StructuredBufferCopy>& copyRegions = {}
	);

	std::vector<vk::BufferCopy>&& toBufferCopy(const std::vector<StructuredBufferCopy>& cpyReg);

	vk::BufferMemoryBarrier genUploadBarrier();

	DescriptorHandler::LayoutBinding getLayoutBinding(vk::ShaderStageFlags stage = vk::ShaderStageFlagBits::eAll) const;
	DescriptorHandler::DescriptorWrite getDescriptorWrite() const;
};

}