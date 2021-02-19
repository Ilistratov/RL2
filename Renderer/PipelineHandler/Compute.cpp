#include "Compute.h"
#include "LoadShaderModule.h"

namespace Renderer::Pipeline {
Compute::Compute(
	const DPoolHandler& dPool,
	const std::vector<vk::PushConstantRange>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain
) {
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

Compute::Compute(Compute&& other) {
	swap(other);
}

void Compute::operator=(Compute&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

void Compute::swap(Compute& other) {
	std::swap(data, other.data);
}

void Compute::free() {
	core.device().destroyPipeline(data.ppln);
	core.device().destroyPipelineLayout(data.layt);

	data.ppln = vk::Pipeline{};
	data.layt = vk::PipelineLayout{};
}

Compute::Data& Compute::getData() {
	return data;
}

const Compute::Data& Compute::getData() const {
	return data;
}

Compute::~Compute() {
	free();
}

}