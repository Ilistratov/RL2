#include "BufferBase.h"

namespace Renderer::ResourceHandler {

BufferBase::BufferBase(
	vk::DeviceSize sz,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags memoryProperties,
	uint64_t reservedBufferDataId) {
	auto device = core.device();

	auto buff = device.createBuffer(
		vk::BufferCreateInfo{
			vk::BufferCreateFlags{},
			sz,
			usage,
			vk::SharingMode::eExclusive, //TODO make it configurable ???
			{},
			{}
		}
	);

	auto mem_requierments = device.getBufferMemoryRequirements(buff);
	auto mem = device.allocateMemory(
		vk::MemoryAllocateInfo{
			mem_requierments.size,
			core.apiBase().findMemoryTypeIndex(mem_requierments.memoryTypeBits, memoryProperties)
		}
	);


	device.bindBufferMemory(buff, mem, 0);

	if (reservedBufferDataId == UINT64_MAX) {
		reservedBufferDataId = core.getBuffers().size();
		core.getBuffers().push_back({});
	}
	bufferDataId = reservedBufferDataId;
	
	core.getBuffers()[bufferDataId] = DataComponent::BufferData{
		mem,
		buff,
		sz
	};
}

DataComponent::BufferData& BufferBase::getBufferData() {
	return core.getBuffers()[bufferDataId];
}

const DataComponent::BufferData& BufferBase::getBufferData() const {
	return core.getBuffers()[bufferDataId];
}

}