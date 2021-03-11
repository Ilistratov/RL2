#include "Renderer\Core.h"
#include "Compute.h"
#include "LoadShaderModule.h"

namespace Renderer::Pipeline {
Compute::Compute(
	const DescriptorHandler::Layout& dLayout,
	const std::vector<ResourceHandler::IPushConstant*>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain
) {
	std::vector<vk::PushConstantRange> pcRanges(pushConstants.size());
	uint64_t cur_offset = 0;
	
	for (uint64_t i = 0; i < pushConstants.size(); i++) {
		pushConstants[i]->setOffset(cur_offset);
		cur_offset += pushConstants[i]->getSize();
		pcRanges[i] = pushConstants[i]->getPCR();
	}

	layt = core.device().createPipelineLayout(
		vk::PipelineLayoutCreateInfo {
			vk::PipelineLayoutCreateFlags{},
			dLayout.getLayouts(),
			pcRanges
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