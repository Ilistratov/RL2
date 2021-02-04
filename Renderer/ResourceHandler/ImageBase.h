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
};

}