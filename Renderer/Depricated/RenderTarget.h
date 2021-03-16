#pragma once

#include "ImageBase.h"
#include "Renderer\CmdRecorder\BlitRecorder.h"
#include "Renderer\DescriptorHandler\Pool.h"

namespace Renderer::ResourceHandler {

class RenderTarget : public ImageBase {
	vk::ImageView imgView = {};

public:
	RenderTarget() = default;

	RenderTarget(
		vk::Extent2D ext,
		vk::Format fmt
	);

	void swap(RenderTarget& other);
	void free();

	RenderTarget(RenderTarget&& other);
	void operator =(RenderTarget&& other);

	~RenderTarget();

	vk::ImageMemoryBarrier genInitBarrier();
	vk::ImageMemoryBarrier genPreBlitBarrier();
	vk::ImageMemoryBarrier genPostBlitBarrier();
	
	vk::ImageBlit genBlit();

	CmdRecorder::BlitTarget getBlitTarget();
	//void recordBlit(vk::CommandBuffer cb, ImageBase::Data dst);

	DescriptorHandler::LayoutBinding getLayoutBinding(vk::ShaderStageFlags stage = vk::ShaderStageFlagBits::eAll) const;
	DescriptorHandler::DescriptorWrite getDescriptorWrite() const;
};

}