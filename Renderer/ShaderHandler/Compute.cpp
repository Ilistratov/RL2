#include "Compute.h"

namespace Renderer::ShaderHandler {

Compute::Compute(
	ComputeFactory&& factory,
	const std::vector<vk::PushConstantRange>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain,
	uint32_t dispatchX,
	uint32_t dispatchY,
	uint32_t dispatchZ,
	uint64_t reservedPipelineDataId,
	uint64_t reservedDPoolDataId,
	uint64_t reservedCmdPoolDataId
) : dPool(factory.layoutFactory, reservedDPoolDataId),
	pipeline(dPool, pushConstants, shaderFilePath, shaderMain, reservedPipelineDataId),
	cmdPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute)) {
	vcCallback = factory.vcCallback;

	submitFinished = core.device().createFence(vk::FenceCreateInfo{});
	dispatchFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo{});

	cmdPool.reserve(4);
	auto cbInit = cmdPool.getData().cmd.back();
	auto cbDispatch = cmdPool.getData().cmd[0];
	auto cbTransfer = cmdPool.getData().cmd[1];
	
	cbInit.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);

	for (auto callback : vcCallback) {
		callback->onInitRecord(cbInit);
	}

	cbInit.end();

	core.apiBase().computeQueue().submit(
		vk::SubmitInfo{
			{},
			{},
			cbInit,
			{}
		},
		submitFinished
	);

	cbDispatch.begin(vk::CommandBufferBeginInfo{});
	
	cbDispatch.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eComputeShader,
		vk::DependencyFlags{},
		{},
		factory.preDispatchBrrBatch.buff,
		factory.preDispatchBrrBatch.img
	);

	cbDispatch.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getData().ppln);
	cbDispatch.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.getData().layt, 0, dPool.getData().sets, {});
	cbDispatch.dispatch(dispatchX, dispatchY, dispatchZ);

	cbDispatch.end();

	cbTransfer.begin(vk::CommandBufferBeginInfo{});

	cbTransfer.pipelineBarrier(
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags{},
		{},
		factory.preTransferBrrBatch.buff,
		factory.preTransferBrrBatch.img
	);

	for (auto callback : vcCallback) {
		callback->onTransferRecord(cbTransfer);
	}

	cbTransfer.end();
}

//TODO
//regular preDispatch command buffer recording and submission is not implemented yet
void Compute::submit() {
	core.device().waitForFences(submitFinished, VK_TRUE, UINT64_MAX);

	std::vector<vk::Semaphore> dispatch_signal = { dispatchFinished };

	vk::SubmitInfo dispatchSubmit{
		{},
		{},
		cmdPool.getData().cmd[0],
		dispatch_signal
	};

	std::vector<vk::Semaphore> transfer_wait = dispatch_signal;
	std::vector<vk::PipelineStageFlags> transfer_wait_stage = { vk::PipelineStageFlagBits::eTransfer };

	vk::SubmitInfo transferSubmit{
		transfer_wait,
		transfer_wait_stage,
		cmdPool.getData().cmd[1],
		{}
	};

	core.apiBase().computeQueue().submit({ dispatchSubmit, transferSubmit }, submitFinished);
}

//TODO
//Probably should switch from storing ALL the data components
//in the RendererCore, since for some of them
//it makes more sense to store them
//directly where they are used
//like in this case with cmdPool
Compute::~Compute() {
	core.device().waitForFences(submitFinished, VK_TRUE, UINT64_MAX);
	core.device().destroyFence(submitFinished);
	core.device().destroySemaphore(dispatchFinished);
}

}