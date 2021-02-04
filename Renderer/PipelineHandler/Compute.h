#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Renderer\Core.h"
#include "DPoolHandler.h"

namespace Renderer::Pipeline {

class Compute {
	uint64_t pipelineDataId = UINT64_MAX;
	//TODO create pipeline using DescriptorHandler for layout creation

public:
	Compute() = default;
	Compute(
		const DPoolHandler& dPool,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain,
		uint64_t reservedPipelineDataId = UINT64_MAX
	);

	DataComponent::PipelineData& getData();
};

}