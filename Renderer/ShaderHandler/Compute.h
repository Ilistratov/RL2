#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "ComputeFactory.h"

namespace Renderer::ShaderHandler {

class Compute {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	ResourceHandler::CommandPool cmdPool;

	vk::Fence submitFinished;
	vk::Semaphore dispatchFinished;

	std::vector<IVarControllerCallback*> vcCallback;
public:
	Compute() = default;
	Compute(
		ComputeFactory&& factory,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain,
		uint32_t dispatchX,
		uint32_t dispatchY,
		uint32_t dispatchZ,
		uint64_t reservedPipelineDataId = UINT64_MAX,
		uint64_t reservedDPoolDataId = UINT64_MAX,
		uint64_t reservedCmdPoolDataId = UINT64_MAX
	);

	void submit();

	~Compute();
};

}