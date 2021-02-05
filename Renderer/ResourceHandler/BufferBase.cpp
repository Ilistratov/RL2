#include "BufferBase.h"

namespace Renderer::ResourceHandler {

BufferBase::BufferBase(
	vk::DeviceSize sz,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags memoryProperties,
	uint64_t reservedBufferDataId) {
	if (reservedBufferDataId == UINT64_MAX) {
		reservedBufferDataId = core.getBuffers().size();
		core.getBuffers().push_back({});
	}
	
	bufferDataId = reservedBufferDataId;
	auto device = core.device();

	auto& data = getData();

	data.buff = device.createBuffer(
		vk::BufferCreateInfo{
			vk::BufferCreateFlags{},
			sz,
			usage,
			vk::SharingMode::eExclusive, //TODO make it configurable ???
			{},
			{}
		}
	);

	auto mem_requierments = device.getBufferMemoryRequirements(data.buff);
	
	data.sz = mem_requierments.size;
	data.mem = device.allocateMemory(
		vk::MemoryAllocateInfo{
			mem_requierments.size,
			core.apiBase().findMemoryTypeIndex(mem_requierments.memoryTypeBits, memoryProperties)
		}
	);

	device.bindBufferMemory(data.buff, data.mem, 0);
}

DataComponent::BufferData& BufferBase::getData() {
	return core.getBuffers()[bufferDataId];
}

const DataComponent::BufferData& BufferBase::getData() const {
	return core.getBuffers()[bufferDataId];
}

}