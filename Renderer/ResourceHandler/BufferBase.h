#pragma once

#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class BufferBase {
public:
	struct Data {
		vk::DeviceMemory mem = vk::DeviceMemory{};
		vk::Buffer buff = vk::Buffer{};

		vk::DeviceSize sz = 0;
	};

	BufferBase(const BufferBase&) = delete;
	BufferBase& operator =(const BufferBase&) = delete;

	BufferBase() = default;
	BufferBase(
		vk::DeviceSize sz,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties
	);

	BufferBase(BufferBase&& other);
	void operator =(BufferBase&& other);

	void swap(BufferBase& other);

	Data& getData();
	const Data& getData() const;

	void free();

	~BufferBase();
protected:
	Data data;

	vk::BufferMemoryBarrier genMemoryBarrier(vk::AccessFlags srcAccess, vk::AccessFlags dstAccess);
};

}