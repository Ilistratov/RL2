#pragma once

#include <vector>

#include "BufferBase.h"

namespace Renderer::ResourceHandler {

class StagingBuffer : public BufferBase {
public:
	StagingBuffer(uint64_t sz);

	void copyFrom(void* src, uint64_t sz = 0, uint64_t dstOffset = 0);

	template<typename T>
	inline void copyFrom(const std::vector<T>& src, uint64_t dstOffset = 0) {
		copyFrom((void*)src.data(), src.size() * sizeof(T), dstOffset);
	}

	void recordCopyTo(vk::CommandBuffer& cmd, BufferBase& dst, const std::vector<vk::BufferCopy>& copyRegions = {});
};

}