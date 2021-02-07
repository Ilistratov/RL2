#pragma once

#include "ImageBase.h"

namespace Renderer::ResourceHandler {

class RenderTarget : public ImageBase {
public:
	RenderTarget(
		vk::Extent2D ext,
		vk::Format fmt,
		uint64_t reservedImageDataId = UINT64_MAX
	);

	vk::ImageMemoryBarrier genPreRenderBarrier();
	vk::ImageMemoryBarrier genPreBlitBarrier();
	
	vk::ImageBlit genBlit();
	void recordBlit(vk::CommandBuffer cb, DataComponent::ImageData& dst);
};

}