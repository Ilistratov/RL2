#include <future>
#include "Compute.h"

namespace Renderer::ShaderHandler {

vk::CommandBuffer Compute::recordInit() {
	vk::CommandBuffer initCB = cmdPool.reserveOneTimeSubmit();
	
	initCB.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);

	bnd->recordInit(initCB);

	initCB.end();

	return initCB;
}

void Compute::waitSubmissionFin() {
	core.device().waitForFences(submissionFinished, true, UINT64_MAX);
}

void Compute::resetSubmissionFin() {
	core.device().resetFences(submissionFinished);
}

void Compute::recordLoadData() {
	auto barriers = bnd->collectPreLoadDataBarriers();
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[0];

	cmd.begin(vk::CommandBufferBeginInfo{});
	
	barriers.record(
		cmd,
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer
	);
	bnd->recordLoadData(cmd);

	cmd.end();
}

void Compute::recordDispatch() {
	auto barriers = bnd->collectPreDispatchBarriers();
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[1];
	
	cmd.begin(vk::CommandBufferBeginInfo{});
	
	barriers.record(
		cmd,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eComputeShader
	);

	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getData().ppln);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.getData().layt, 0, dPool.getData().sets, {});
	auto [x, y, z] = dispatchDim;
	cmd.dispatch(x, y, z);

	cmd.end();
}

void Compute::recordTransferResult() {
	auto barriers = bnd->collectPreTransferResultBarriers();
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[2];

	cmd.begin(vk::CommandBufferBeginInfo{});

	barriers.record(
		cmd,
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer
	);
}

Compute::Compute(
	ShaderBindable::IShaderBindable* bnd, // only one as inside it can hold as much complexity as we want
	const std::string& shaderFilePath,
	const std::string& shaderMain,
	std::tuple<int, int, int> dispatchDim
) : bnd(bnd), dispatchDim(dispatchDim) {
	dPool = Pipeline::DPoolHandler(bnd->collectSetBindables());
	
	pipeline = Pipeline::Compute(
		dPool,
		bnd->collectPushConstants(),
		shaderFilePath,
		shaderMain
	);

	submissionFinished = core.device().createFence(
		vk::FenceCreateInfo{
			vk::FenceCreateFlagBits::eSignaled
		}
	);

	auto initCB = recordInit();

	core.apiBase().computeQueue().submit(
		vk::SubmitInfo{
			{},
			{},
			initCB,
			{}
		},
		submissionFinished
	);

	cmdPool = ResourceHandler::CommandPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute));
	cmdPool.reserve(3);

	auto ldAsync = std::async(&Compute::recordLoadData, this);
	auto dAsync = std::async(&Compute::recordDispatch, this);
	auto trAsync = std::async(&Compute::recordTransferResult, this);

	loadDataFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo{});
	dispatchFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo{});

	loadDataDynamicCB = cmdPool.reserveOneTimeSubmit();
	transferResultDynamicCB = cmdPool.reserveOneTimeSubmit();

	waitSubmissionFin();
	cmdPool.freeOneTimeSubmit({ initCB });

	ldAsync.wait();
	dAsync.wait();
	trAsync.wait();
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

void Compute::swap(Compute& other) {
	waitSubmissionFin();
	other.waitSubmissionFin();
	
	dPool.swap(other.dPool);
	pipeline.swap(other.pipeline);
	cmdPool.swap(other.cmdPool);

	std::swap(loadDataDynamicCB, other.loadDataDynamicCB);
	std::swap(transferResultDynamicCB, other.transferResultDynamicCB);

	std::swap(bnd, other.bnd);
	std::swap(dispatchDim, other.dispatchDim);
	
	std::swap(submissionFinished, other.submissionFinished);
	std::swap(loadDataFinished, other.loadDataFinished);
	std::swap(dispatchFinished, other.dispatchFinished);
}

void Compute::free() {
	waitSubmissionFin();

	dPool.free();
	pipeline.free();

	cmdPool.freeOneTimeSubmit({ loadDataDynamicCB, transferResultDynamicCB });
	cmdPool.free();

	bnd = nullptr; //it's only a callback pointer, so we are not responsible for this resource
	dispatchDim = { 0, 0, 0 };
	
	core.device().destroyFence(submissionFinished);
	core.device().destroySemaphore(loadDataFinished);
	core.device().destroySemaphore(dispatchFinished);

	submissionFinished = vk::Fence{};
	loadDataFinished = vk::Semaphore();
	dispatchFinished = vk::Semaphore();
}

void Compute::dispatch() {
	waitSubmissionFin();
	
	loadDataDynamicCB.reset();
	bnd->recordLoadDataDynamic(loadDataDynamicCB);
	
	transferResultDynamicCB.reset();
	bnd->recordTransferResultDynamic(transferResultDynamicCB);

	std::vector<vk::CommandBuffer> loadDataSubmitCB = { cmdPool.getData().cmd[0], loadDataDynamicCB };

	vk::SubmitInfo loadDataSubmit{
		{},
		{},
		loadDataDynamicCB,
		loadDataFinished
	};

	vk::SubmitInfo dispatchSubmit = {
		loadDataFinished,
		vk::PipelineStageFlags(vk::PipelineStageFlagBits::eComputeShader),
		cmdPool.getData().cmd[1],
		dispatchFinished
	};

	std::vector<vk::CommandBuffer> transferResultSubmitCB = { cmdPool.getData().cmd[2], transferResultDynamicCB };

	vk::SubmitInfo transferResultSubmit = {
		dispatchFinished,
		vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer),
		transferResultSubmitCB,
		{}
	};

	resetSubmissionFin();
	core.apiBase().computeQueue().submit({ loadDataSubmit, dispatchSubmit, transferResultSubmit }, submissionFinished);
}

Compute::~Compute() {
	free();
}

}