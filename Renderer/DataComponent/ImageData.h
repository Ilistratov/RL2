#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DataComponent {

struct ImageData {
	vk::DeviceMemory mem;
	vk::Image img;

	vk::DeviceSize sz = 0;
	vk::Extent2D ext = { 0, 0 };
	vk::Format fmt = vk::Format::eUndefined;
};

}