#pragma once

#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class ImageBase {
	uint64_t imageDataId = UINT64_MAX;

protected:
	vk::ImageMemoryBarrier genLayoutTransitionBarrier(
		vk::ImageLayout srcLayt,
		vk::ImageLayout dstLayt,
		vk::AccessFlags srcAccess,
		vk::AccessFlags dstAccess
	);

	vk::ImageMemoryBarrier genTransferSrcBarrier(
		vk::ImageLayout srcLayt
	);

	vk::ImageMemoryBarrier genTransferDstBarrier(
		vk::ImageLayout srcLayt
	);

	vk::ImageMemoryBarrier genShaderRWBarrier(
		vk::ImageLayout srcLayt
	);

public:
	ImageBase() = default;
	ImageBase(
		vk::Extent2D ext,
		vk::Format fmt,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		uint64_t reservedImageDataId = UINT64_MAX,
		vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined
	);
	
	DataComponent::ImageData& getData();
	vk::ImageSubresourceLayers getSubresourceLayers() const;
	vk::ImageSubresourceRange getSubresourceRange() const;
};

}