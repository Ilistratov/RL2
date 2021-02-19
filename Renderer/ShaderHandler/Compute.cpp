#include "Compute.h"

namespace Renderer::ShaderHandler {



vk::Fence Compute::recordAndSubmitInitCB() {
	vk::CommandBuffer initCB = cmdPool.reserveOneTimeSubmit();
	
	initCB.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);

	bnd->recordInit(initCB);

	initCB.end();

	vk::Fence initFinished = core.device().createFence(vk::FenceCreateInfo{});

	core.apiBase().computeQueue().submit(
		vk::SubmitInfo{
			{},
			{},
			initCB,
			{}
		},
		initFinished
	);

	return initFinished;
}

Compute::Compute(
	IShaderBindable* bnd, // only one as composition can be done inside of the passed instance and we don't need to know about it.
	const std::string& shaderFilePath,
	const std::string& shaderMain
) : bnd(bnd) {
	dPool = Pipeline::DPoolHandler(bnd->collectSetBindables());
	
	pipeline = Pipeline::Compute(
		dPool,
		bnd->collectPushConstants(),
		shaderFilePath,
		shaderMain
	);

	auto initFinished = recordAndSubmitInitCB();

	cmdPool = ResourceHandler::CommandPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute));
	cmdPool.reserve(3);
	
	//TODO
}

}