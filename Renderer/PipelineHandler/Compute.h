#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Renderer\Core.h"

namespace Renderer::Pipeline {

class Compute {
	uint64_t PipelineDataId;
	//TODO create pipeline using DescriptorHandler for layout creation
};

}