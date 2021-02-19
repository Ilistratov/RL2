#include "ImageBase.h"

namespace Renderer::ResourceHandler {

ImageBase::ImageBase(
	vk::Extent2D ext,
	vk::Format fmt,
	vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags memoryProperties,
	vk::ImageLayout initialLayout
) {
	data.ext = ext;
	data.fmt = fmt;
	
	data.img = core.device().createImage(
		vk::ImageCreateInfo{
			vk::ImageCreateFlags{},
			vk::ImageType::e2D,
			fmt,
			vk::Extent3D{ext, 1},
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			usage,
			vk::SharingMode::eExclusive, //TODO make it configurable ???
			0,
			nullptr,
			initialLayout
		}
	);
	
	auto mem_requierments = core.device().getImageMemoryRequirements(data.img);
	
	data.sz = mem_requierments.size;
	
	data.mem = core.device().allocateMemory(
		vk::MemoryAllocateInfo{
			mem_requierments.size,
			core.apiBase().findMemoryTypeIndex(mem_requierments.memoryTypeBits, memoryProperties)
		}
	);

	core.device().bindImageMemory(data.img, data.mem, 0);
}

ImageBase::ImageBase(ImageBase&& other) {
	swap(other);
}

void ImageBase::operator=(ImageBase&& other) {
	swap(other);
	other.free();
}

void ImageBase::swap(ImageBase& other) {
	if (this == &other) {
		return;
	}

	std::swap(data, other.data);
}

void ImageBase::free() {
	core.device().destroyImage(data.img);
	core.device().freeMemory(data.mem);

	data.sz = 0;
	data.ext = vk::Extent2D{ 0, 0 };
	data.fmt = vk::Format::eUndefined;

	data.mem = vk::DeviceMemory{};
	data.img = vk::Image{};
}

ImageBase::Data& ImageBase::getData() {
	return data;
}

const ImageBase::Data& ImageBase::getData() const {
	return data;
}

vk::ImageSubresourceLayers ImageBase::getSubresourceLayers() const {
	return vk::ImageSubresourceLayers{
		vk::ImageAspectFlagBits::eColor,
		0,
		0,
		1
	};
}

vk::ImageSubresourceRange ImageBase::getSubresourceRange() const {
	return vk::ImageSubresourceRange{
		vk::ImageAspectFlagBits::eColor,
		0,
		1,
		0,
		1
	};
}

ImageBase::~ImageBase() {
	free();
}

vk::ImageMemoryBarrier ImageBase::genLayoutTransitionBarrier(
	vk::ImageLayout srcLayt,
	vk::ImageLayout dstLayt,
	vk::AccessFlags srcAccess,
	vk::AccessFlags dstAccess
) {
	return vk::ImageMemoryBarrier{
		srcAccess,
		dstAccess,
		srcLayt,
		dstLayt,
		{VK_QUEUE_FAMILY_IGNORED},
		{VK_QUEUE_FAMILY_IGNORED},
		data.img,
		vk::ImageSubresourceRange{
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		}
	};
}

vk::ImageMemoryBarrier ImageBase::genTransferSrcBarrier(
	vk::ImageLayout srcLayt
) {
	return genLayoutTransitionBarrier(
		srcLayt,
		vk::ImageLayout::eTransferSrcOptimal,
		vk::AccessFlagBits::eShaderWrite,
		vk::AccessFlagBits::eTransferRead
	);
}

vk::ImageMemoryBarrier ImageBase::genTransferDstBarrier(
	vk::ImageLayout srcLayt
) {
	return genLayoutTransitionBarrier(
		srcLayt,
		vk::ImageLayout::eTransferDstOptimal,
		vk::AccessFlagBits::eShaderRead,	// doesn't have eShaderWrite since writing to the data that we are going to
		vk::AccessFlagBits::eTransferWrite	// overwrite is somthing strange and it shouldn't happen anyway
	);
}

vk::ImageMemoryBarrier ImageBase::genShaderRWBarrier(
	vk::ImageLayout srcLayt
) {
	return genLayoutTransitionBarrier(
		srcLayt,
		vk::ImageLayout::eGeneral,
		vk::AccessFlagBits::eTransferRead | vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite
	);
}

}