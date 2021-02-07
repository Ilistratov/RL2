#include "RenderTarget.h"

namespace Renderer::ResourceHandler {

RenderTarget::RenderTarget(
	vk::Extent2D ext,
	vk::Format fmt,
	uint64_t reservedImageDataId
) : ImageBase(
	ext,
	fmt,
	vk::ImageUsageFlagBits::eStorage |
	vk::ImageUsageFlagBits::eTransferSrc,
	vk::MemoryPropertyFlagBits::eDeviceLocal,
	reservedImageDataId,
	vk::ImageLayout::eGeneral
) {}

vk::ImageMemoryBarrier RenderTarget::genPreRenderBarrier() {
	return genShaderRWBarrier(vk::ImageLayout::eTransferSrcOptimal);
}

vk::ImageMemoryBarrier RenderTarget::genPreBlitBarrier() {
	return genTransferSrcBarrier(vk::ImageLayout::eGeneral);
}

vk::ImageBlit RenderTarget::genBlit() {
	return vk::ImageBlit{
		getSubresourceLayers(),
		{ vk::Offset3D{0, 0, 0 }, vk::Offset3D(getData().ext.width, getData().ext.height, 1) },
		getSubresourceLayers(),
		{ vk::Offset3D{0, 0, 0}, vk::Offset3D(getData().ext.width, getData().ext.height, 1) }
	};
}

void RenderTarget::recordBlit(vk::CommandBuffer cb, DataComponent::ImageData& dst) {
	cb.blitImage(
		getData().img,
		vk::ImageLayout::eTransferSrcOptimal,
		dst.img,
		vk::ImageLayout::eTransferDstOptimal,
		genBlit(),
		vk::Filter::eLinear
	);
}

}