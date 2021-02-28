#include <future>
#include "Compute.h"

/*
TODO: as now task of work submission has been transfered to separate class,
need to rewrite ShaderHandler::Compute to free it from this task.
Also existing ShaderBindable's should now be derived from ICmdRecorder instead of IShaderInput(Output).
*/

namespace Renderer::ShaderHandler {

Compute::Compute(
	const std::vector<Pipeline::SetBindable>& bnd,
	ShaderBindable::PushConstantController& pcController,
	const std::string& shaderFilePath,
	const std::string& shaderMain,
	std::tuple<int, int, int> dispatchDim
) : dispatchDim(dispatchDim) {
	dPool = Pipeline::DPoolHandler(bnd);
	
	pipeline = Pipeline::Compute(
		dPool,
		pcController.getPCR(),
		shaderFilePath,
		shaderMain
	);

	pcController.bindLayout(pipeline.getData().layt);

	auto initFinished = core.device().createFence(
		vk::FenceCreateInfo{
			vk::FenceCreateFlagBits::eSignaled
		}
	);
}

Compute::Compute(Compute&& other) {
	swap(other);
}

void Compute::operator =(Compute&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

void Compute::recordRegular(vk::CommandBuffer cmd) {
	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getData().ppln);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.getData().layt, 0, dPool.getData().sets, {});
	auto [x, y, z] = dispatchDim;
	cmd.dispatch(x, y, z);
}

void Compute::swap(Compute& other) {
	dPool.swap(other.dPool);
	pipeline.swap(other.pipeline);
	
	std::swap(dispatchDim, other.dispatchDim);
}

void Compute::free() {
	dPool.free();
	pipeline.free();

	dispatchDim = { 0, 0, 0 };
}

Compute::~Compute() {
	free();
}

}
