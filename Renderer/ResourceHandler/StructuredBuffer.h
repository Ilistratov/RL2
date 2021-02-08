#pragma once

#include <vector>

#include "BufferBase.h"
#include "Renderer\PipelineHandler\BindingInfo.h"

namespace Renderer::ResourceHandler {

struct StructuredBufferCopy {
	uint64_t srcOffset = 0, dstOffset = 0, count = 0;
	vk::BufferCopy operator()(uint64_t stride) const;
};

class StructuredBuffer : public BufferBase {
	uint64_t stride = 1, count = 0;
public:
	StructuredBuffer() = default;

	StructuredBuffer(
		uint64_t count,
		uint64_t stride = 1,
		vk::BufferUsageFlagBits specificUsage = {},
		uint64_t reservedBufferDataId = UINT64_MAX
	);

	void recordCopyFrom(
		vk::CommandBuffer cb,
		const DataComponent::BufferData& src,
		const std::vector<StructuredBufferCopy>& copyRegions = {}
	);

	Pipeline::BindingInfo genBindingInfo();
};

}