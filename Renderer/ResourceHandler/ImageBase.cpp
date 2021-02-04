#include "ImageBase.h"

namespace Renderer::ResourceHandler {

ImageBase::ImageBase(
	vk::Extent2D ext,
	vk::Format fmt,
	vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags memoryProperties,
	uint64_t reservedImageDataId
) {
	if (reservedImageDataId == UINT64_MAX) {
		reservedImageDataId = core.getImages().size();
		core.getImages().push_back({});
	}

	DataComponent::ImageData& data = core.getImages()[reservedImageDataId];

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
			vk::ImageLayout::eUndefined
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

DataComponent::ImageData& ImageBase::getData() {
	return core.getImages()[imageDataId];
}

}