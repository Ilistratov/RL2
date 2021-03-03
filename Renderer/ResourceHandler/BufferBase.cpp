#include "BufferBase.h"

namespace Renderer::ResourceHandler {

BufferBase::BufferBase(
	vk::DeviceSize sz,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags memoryProperties) {
	auto device = core.device();
	
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
	
	data.sz = sz;
	data.mem = device.allocateMemory(
		vk::MemoryAllocateInfo{
			mem_requierments.size,
			core.apiBase().findMemoryTypeIndex(mem_requierments.memoryTypeBits, memoryProperties)
		}
	);

	device.bindBufferMemory(data.buff, data.mem, 0);
}

BufferBase::BufferBase(BufferBase&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
}

void BufferBase::operator=(BufferBase&& other) {
	swap(other);
	other.free();
}

void BufferBase::swap(BufferBase& other) {
	std::swap(data, other.data);
}

BufferBase::Data& BufferBase::getData() {
	return data;
}

const BufferBase::Data& BufferBase::getData() const {
	return data;
}

void BufferBase::free() {
	core.device().destroyBuffer(data.buff);
	core.device().freeMemory(data.mem);
	
	data.sz = 0;
	data.buff = vk::Buffer{};
	data.mem = vk::DeviceMemory{};
}

BufferBase::~BufferBase() {
	free();
}

vk::BufferMemoryBarrier BufferBase::genMemoryBarrier(vk::AccessFlags srcAccess, vk::AccessFlags dstAccess) {
	return vk::BufferMemoryBarrier{
		srcAccess,
		dstAccess,
		{VK_QUEUE_FAMILY_IGNORED},
		{VK_QUEUE_FAMILY_IGNORED},
		data.buff,
		0,
		data.sz
	};
}

}