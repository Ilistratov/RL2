#pragma once

#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class BufferBase {
	uint64_t bufferDataId = UINT64_MAX;

public:
	BufferBase() = default;

	BufferBase(
		vk::DeviceSize sz,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		uint64_t reservedBufferDataId = UINT64_MAX
	);

	DataComponent::BufferData& getBufferData();
	const DataComponent::BufferData& getBufferData() const;
};

}