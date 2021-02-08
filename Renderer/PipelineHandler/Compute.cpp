#include "Compute.h"
#include "LoadShaderModule.h"

namespace Renderer::Pipeline {
Compute::Compute(
	const DPoolHandler& dPool,
	const std::vector<vk::PushConstantRange>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain,
	uint64_t reservedPipelineDataId
) {
	if (reservedPipelineDataId == UINT64_MAX) {
		reservedPipelineDataId = core.getPipelines().size();
		core.getPipelines().push_back({});
	}

	pipelineDataId = reservedPipelineDataId;
	auto& data = core.getPipelines()[pipelineDataId];

	data.layt = core.device().createPipelineLayout(
		vk::PipelineLayoutCreateInfo{
			vk::PipelineLayoutCreateFlags{},
			(uint32_t)dPool.getData().layts.size(),
			dPool.getData().layts.data(),
			(uint32_t)pushConstants.size(),
			pushConstants.data()
		}
	);

	auto shaderModule = loadShader(shaderFilePath);
	auto res = core.device().createComputePipeline(
		vk::PipelineCache{},
		vk::ComputePipelineCreateInfo{
			vk::PipelineCreateFlags{},
			vk::PipelineShaderStageCreateInfo{
				vk::PipelineShaderStageCreateFlags{},
				vk::ShaderStageFlagBits::eCompute,
				shaderModule.get(),
				shaderMain.c_str(),
				{}
			},
			data.layt,
			{},
			-1
		}
	);

	if (res.result != vk::Result::eSuccess) {
		throw std::runtime_error(
			gen_err_str(__FILE__, __LINE__,
				"Failed to create compute pipeline, error code: " + std::to_string((uint64_t)res.result)
			)
		);
	}

	data.ppln = res.value;
}

DataComponent::PipelineData& Compute::getData() {
	return core.getPipelines()[pipelineDataId];
}

}