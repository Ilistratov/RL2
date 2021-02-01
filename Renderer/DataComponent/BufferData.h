#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DataComponent {

struct BufferData {
	vk::DeviceMemory mem;
	vk::Buffer buff;

	vk::DeviceSize sz;
};

}