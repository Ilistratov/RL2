#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::ResourceHandler {

class ImageBase {
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
	struct Data {
		vk::DeviceMemory mem = vk::DeviceMemory{};
		vk::Image img = vk::Image{};

		vk::DeviceSize sz = 0;
		vk::Extent2D ext = { 0, 0 };
		vk::Format fmt = vk::Format::eUndefined;
	};

	ImageBase(const ImageBase&) = delete;
	ImageBase& operator =(const ImageBase&) = delete;

	ImageBase() = default;
	ImageBase(
		vk::Extent2D ext,
		vk::Format fmt,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined
	);

	ImageBase(ImageBase&& other);
	void operator =(ImageBase&& other);

	void swap(ImageBase& other);
	void free();
	
	Data& getData();
	const Data& getData() const;
	
	vk::ImageSubresourceLayers getSubresourceLayers() const;
	vk::ImageSubresourceRange getSubresourceRange() const;

	vk::ImageView createImageView();

	~ImageBase();
protected:
	Data data;
};

}