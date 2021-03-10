#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Layout.h"

namespace Renderer::DescriptorHandler {

struct DescriptorWrite {
	uint32_t dstArrayElement = {};
	uint32_t descriptorCount = {};
	vk::DescriptorType type = {};
	std::vector<vk::DescriptorImageInfo> imageInfo = {};
	std::vector<vk::DescriptorBufferInfo> bufferInfo = {};
	//std::vector<vk::BufferView> texelBufferView;
};

class Pool {
	vk::DescriptorPool pool;
	std::vector<vk::DescriptorSet> sets;

public:
	Pool() = default;

	Pool(const Layout& layt);

	Pool(const Pool&) = delete;
	Pool& operator =(const Pool&) = delete;

	void swap(Pool& other);
	void free();

	Pool(Pool&& other);
	void operator =(Pool&& other);

	~Pool();

	//DescriptorWrite with descriptorCount = 0 can be supplied, in that case
	//nothing will be written in the specified set binding
	void write(uint32_t setId, uint32_t bindingId, const DescriptorWrite& dbWrite);

	//DescriptorWrite with descriptorCount = 0 can be supplied, in that case
	//nothing will be written in the specified set binding
	void write(uint32_t setId, const std::vector<DescriptorWrite>& dWrites);

	//DescriptorWrite with descriptorCount = 0 can be supplied, in that case
	//nothing will be written in the specified set binding
	void write(const std::vector<std::vector<DescriptorWrite>>& sWrites);

	const std::vector<vk::DescriptorSet>& getSets() const;
};

}