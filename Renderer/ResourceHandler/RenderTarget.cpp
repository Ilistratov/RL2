#include "Renderer\Core.h"
#include "RenderTarget.h"

namespace Renderer::ResourceHandler {

RenderTarget::RenderTarget(
	vk::Extent2D ext,
	vk::Format fmt
) : ImageBase(
	ext,
	fmt,
	vk::ImageUsageFlagBits::eStorage |
	vk::ImageUsageFlagBits::eTransferSrc,
	vk::MemoryPropertyFlagBits::eDeviceLocal
), imgView(ImageBase::createImageView()) {}

void RenderTarget::swap(RenderTarget& other) {
	ImageBase::swap(other);
	std::swap(imgView, other.imgView);
}

void RenderTarget::free() {
	core.device().destroyImageView(imgView);
	ImageBase::free();
}

RenderTarget::RenderTarget(RenderTarget&& other) {
	swap(other);
}

void RenderTarget::operator =(RenderTarget&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

RenderTarget::~RenderTarget() {
	free();
}

vk::ImageMemoryBarrier RenderTarget::genInitBarrier() {
	return genLayoutTransitionBarrier( 
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eGeneral,
		{},
		vk::AccessFlagBits::eShaderRead |
		vk::AccessFlagBits::eShaderWrite
	);
}

vk::ImageMemoryBarrier RenderTarget::genPreBlitBarrier() {
	return genLayoutTransitionBarrier(
		vk::ImageLayout::eGeneral,
		vk::ImageLayout::eTransferSrcOptimal,
		vk::AccessFlagBits::eShaderWrite,
		vk::AccessFlagBits::eTransferRead
	);
}

vk::ImageMemoryBarrier RenderTarget::genPostBlitBarrier() {
	return genLayoutTransitionBarrier(
		vk::ImageLayout::eTransferSrcOptimal,
		vk::ImageLayout::eGeneral,
		vk::AccessFlagBits::eTransferRead,
		vk::AccessFlagBits::eShaderWrite
	);
}

vk::ImageBlit RenderTarget::genBlit() {
	return vk::ImageBlit{
		getSubresourceLayers(),
		{ vk::Offset3D{0, 0, 0 }, vk::Offset3D(getData().ext.width, getData().ext.height, 1) },
		getSubresourceLayers(),
		{ vk::Offset3D{0, 0, 0}, vk::Offset3D(getData().ext.width, getData().ext.height, 1) }
	};
}

CmdRecorder::BlitTarget RenderTarget::getBlitTarget() {
	return CmdRecorder::BlitTarget{
		data.img,
		data.ext,
		genPreBlitBarrier(),
		genPostBlitBarrier()
	};
}

/*
void RenderTarget::recordBlit(vk::CommandBuffer cb, ImageBase::Data dst) {
	cb.blitImage(
		getData().img,
		vk::ImageLayout::eTransferSrcOptimal,
		dst.img,
		vk::ImageLayout::eTransferDstOptimal,
		genBlit(),
		vk::Filter::eLinear
	);
}
*/

DescriptorHandler::LayoutBinding RenderTarget::getLayoutBinding(vk::ShaderStageFlags stage) const {
	return DescriptorHandler::LayoutBinding{
		vk::DescriptorType::eStorageImage,
		1,
		stage
	};
}

DescriptorHandler::DescriptorWrite RenderTarget::getDescriptorWrite() const {
	return DescriptorHandler::DescriptorWrite{
		0,
		1,
		vk::DescriptorType::eStorageImage,
		{ vk::DescriptorImageInfo{ {}, imgView, vk::ImageLayout::eGeneral } }
	};
}

}