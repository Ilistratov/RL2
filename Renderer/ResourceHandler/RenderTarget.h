#pragma once

#include "ImageBase.h"

namespace Renderer::ResourceHandler {

class RenderTarget : public ImageBase {
public:
	RenderTarget() = default;

	RenderTarget(
		vk::Extent2D ext,
		vk::Format fmt,
		vk::ImageLayout initialLayout = vk::ImageLayout::eGeneral
	);

	vk::ImageMemoryBarrier genPreRenderBarrier();
	vk::ImageMemoryBarrier genPreBlitBarrier();
	
	vk::ImageBlit genBlit();
	void recordBlit(vk::CommandBuffer cb, ImageBase::Data dst);
};

}