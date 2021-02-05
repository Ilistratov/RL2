#pragma once

#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class ImageBase {
	uint64_t imageDataId = UINT64_MAX;
public:
	ImageBase() = default;
	ImageBase(
		vk::Extent2D ext,
		vk::Format fmt,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		uint64_t reservedImageDataId = UINT64_MAX
	);
	
	DataComponent::ImageData& getData();

	vk::ImageMemoryBarrier genLayoutTransitionBarrier(
		vk::CommandBuffer cb,
		vk::ImageLayout srcLayt,
		vk::ImageLayout dstLayt,
		vk::AccessFlags srcAccess,
		vk::AccessFlags dstAccess
	);

	vk::ImageMemoryBarrier genTransferSrcBarrier(
		vk::CommandBuffer cb,
		vk::ImageLayout srcLayt
	);

	vk::ImageMemoryBarrier genTransferDstBarrier(
		vk::CommandBuffer cb,
		vk::ImageLayout srcLayt
	);

	vk::ImageMemoryBarrier genShaderRWBarrier(
		vk::CommandBuffer cb,
		vk::ImageLayout srcLayt
	);

};

}