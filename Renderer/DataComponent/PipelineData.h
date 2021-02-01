#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DataComponent {

struct PipelineData {
	vk::Pipeline ppln;
	vk::PipelineLayout layt;
};

}