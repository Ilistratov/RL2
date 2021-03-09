#include "Renderer\Core.h"
#include "Compute.h"
#include "LoadShaderModule.h"

namespace Renderer::Pipeline {
Compute::Compute(
	const DescriptorHandler::Layout& dLayout,
	const std::vector<vk::PushConstantRange>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain
) {
	layt = core.device().createPipelineLayout(
		vk::PipelineLayoutCreateInfo{
			vk::PipelineLayoutCreateFlags{},
			dLayout.getLayouts(),
			pushConstants
		}
	);

	dPool = DescriptorHandler::Pool(dLayout);

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
			layt,
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

	ppln = res.value;
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
	std::swap(ppln, other.ppln);
	std::swap(layt, other.layt);
	dPool.swap(other.dPool);
}

void Compute::free() {
	core.device().destroyPipeline(ppln);
	core.device().destroyPipelineLayout(layt);
	dPool.free();

	ppln = vk::Pipeline{};
	layt = vk::PipelineLayout{};
}

Compute::~Compute() {
	free();
}

void Compute::bind(vk::CommandBuffer& cmd) {
	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, ppln);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, layt, 0, dPool.getSets(), {});
}

DescriptorHandler::Pool& Compute::getDPool() {
	return dPool;
}

}