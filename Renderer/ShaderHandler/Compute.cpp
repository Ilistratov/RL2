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

	inputCtr->recordInit(initCB);
	outputCtr->recordInit(initCB);

	initCB.end();

	return initCB;
}

void Compute::recordInputRegularCB() {
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[0];

	cmd.begin(vk::CommandBufferBeginInfo{});
	inputCtr->recordRegular(cmd);
	cmd.end();
}

void Compute::recordDispatchCB() {
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[1];
	
	cmd.begin(vk::CommandBufferBeginInfo{});
	
	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getData().ppln);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.getData().layt, 0, dPool.getData().sets, {});
	
	auto [x, y, z] = dispatchDim;
	cmd.dispatch(x, y, z);

	cmd.end();
}

void Compute::recordOutputRegularCB() {
	vk::CommandBuffer& cmd = cmdPool.getData().cmd[2];

	cmd.begin(vk::CommandBufferBeginInfo{});
	outputCtr->recordRegular(cmd);
	cmd.end();
}

void Compute::recordInputDynamicCB() {
	core.device().waitSemaphores(
		vk::SemaphoreWaitInfo{
			{},
			inputFinished,
			inputFinishedWaitVal
		},
		UINT64_MAX
	);

	inputDynamicCB.reset();
	inputCtr->recordDynamic(inputDynamicCB);
}

void Compute::recordOutputDynamicCB() {
	core.device().waitSemaphores(
		vk::SemaphoreWaitInfo{
			{},
			outputFinished,
			outputFinishedWaitVal
		},
		UINT64_MAX
	);

	outputDynamicCB.reset();
	outputCtr->recordDynamic(outputDynamicCB);
}

Compute::Compute(
	ShaderBindable::IShaderInput* inputCtr,
	ShaderBindable::IShaderOutput* outputCtr,
	const std::vector<Pipeline::SetBindable>& bnd,
	const std::vector<vk::PushConstantRange>& pushConstants,
	const std::string& shaderFilePath,
	const std::string& shaderMain,
	std::tuple<int, int, int> dispatchDim
) : inputCtr(inputCtr), outputCtr(outputCtr), dispatchDim(dispatchDim) {
	dPool = Pipeline::DPoolHandler(bnd);
	
	pipeline = Pipeline::Compute(
		dPool,
		pushConstants,
		shaderFilePath,
		shaderMain
	);

	auto initFinished = core.device().createFence(
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
		initFinished
	);

	cmdPool = ResourceHandler::CommandPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute));
	cmdPool.reserve(3);

	auto ldAsync = std::async(&Compute::recordInputRegularCB, this);
	auto dAsync = std::async(&Compute::recordInputRegularCB, this);
	auto trAsync = std::async(&Compute::recordOutputRegularCB, this);

	vk::SemaphoreTypeCreateInfo inputFinishedTCI{
		vk::SemaphoreType::eTimeline,
		0
	};

	vk::SemaphoreTypeCreateInfo dispatchFinishedTCI{
		vk::SemaphoreType::eTimeline,
		0
	};

	vk::SemaphoreTypeCreateInfo outputFinishedTCI{
		vk::SemaphoreType::eTimeline,
		0
	};

	inputFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo().setPNext(inputFinished));
	dispatchFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo().setPNext(dispatchFinished));
	outputFinished = core.device().createSemaphore(vk::SemaphoreCreateInfo().setPNext(outputFinished));

	inputDynamicCB = cmdPool.reserveOneTimeSubmit();
	outputDynamicCB = cmdPool.reserveOneTimeSubmit();

	core.device().waitForFences(initFinished, true, UINT64_MAX);
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
	dPool.swap(other.dPool);
	pipeline.swap(other.pipeline);
	cmdPool.swap(other.cmdPool);

	std::swap(inputDynamicCB, other.inputDynamicCB);
	std::swap(outputDynamicCB, other.outputDynamicCB);

	std::swap(inputCtr, other.inputCtr);
	std::swap(outputCtr, other.outputCtr);
	
	std::swap(dispatchDim, other.dispatchDim);
	
	std::swap(inputFinished, other.inputFinished);
	std::swap(inputFinishedWaitVal, other.inputFinishedWaitVal);
	
	std::swap(dispatchFinished, other.dispatchFinished);
	std::swap(dispatchFinishedWaitVal, other.dispatchFinishedWaitVal);
	
	std::swap(outputFinished, other.outputFinished);
	std::swap(outputFinishedWaitVal, other.outputFinishedWaitVal);
}

void Compute::free() {
	dPool.free();
	pipeline.free();

	cmdPool.freeOneTimeSubmit({ inputDynamicCB, outputDynamicCB });
	inputDynamicCB = vk::CommandBuffer{};
	outputDynamicCB = vk::CommandBuffer{};
	cmdPool.free();

	inputCtr = nullptr; //it's only a callback pointer, so we are not responsible for this resource
	outputCtr = nullptr; //it's only a callback pointer, so we are not responsible for this resource
	dispatchDim = { 0, 0, 0 };
	
	core.device().destroySemaphore(inputFinished);
	inputFinishedWaitVal = 0;

	core.device().destroySemaphore(dispatchFinished);
	dispatchFinishedWaitVal = 0;

	core.device().destroySemaphore(outputFinished);
	outputFinishedWaitVal = 0;

	inputFinished = vk::Semaphore{};
	dispatchFinished = vk::Semaphore{};
	outputFinished = vk::Semaphore{};
}

// WTF
// Input:
//  in order do record input dynamic
//  host waits on inputFinishedWaitVal
//  then it record inputDynamic.
//  on device it waits for previous dispatch to finish(dispatchWaitVal)
//  at this point, inputCB's can
//  be submitted, inputFinishedWaitVal can be incremented
// Dispatch:
//  on device waits for outputCB from previous iteration
//  and inputCB from present iteration to finish (outputFinishedWaitVal, inputFinishedWaitVal)
//  dispatchFinishedWaitVal is incremented
//  it will be signaled when dispatch is done
// Output:
//  just like input, makes host wait for previous
//  output(outputFinishedWaitVal) to finish(bc can't reset outputDynamic while it's being executed)
//  on device waits for dispatchFinished
//  then signals outputFinished with ++outputFinishedWaitVal
void Compute::dispatch() {
	recordInputDynamicCB();
	
	//transferResultDynamicCB.reset();
	//ctr->recordTransferResultDynamic(transferResultDynamicCB);

	std::vector<vk::CommandBuffer> inputSubmitCB = { cmdPool.getData().cmd[0], inputDynamicCB };

	vk::TimelineSemaphoreSubmitInfo inputSubmitTimelineInfo {
		dispatchFinishedWaitVal, //wait
		++inputFinishedWaitVal //signal
	};

	vk::SubmitInfo inputSubmit{
		dispatchFinished,
		vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer),
		inputSubmitCB,
		inputFinished
	};

	inputSubmit.setPNext(&inputSubmitTimelineInfo);

	std::vector<uint64_t> dispatchSubmitWaitVal = {
		inputFinishedWaitVal, //from this iteration
		outputFinishedWaitVal //from previous iteration
	};

	vk::TimelineSemaphoreSubmitInfo dispatchSubmitTimelineInfo{
		dispatchSubmitWaitVal, //wait
		++dispatchFinishedWaitVal //signal
	};

	std::vector<vk::Semaphore> dispatchSubmitWaitSemaphores = {
		inputFinished,
		outputFinished
	};

	std::vector<vk::PipelineStageFlags> dispatchSubmitWaitStageFlags = {
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eComputeShader
	};

	vk::SubmitInfo dispatchSubmit = {
		dispatchSubmitWaitSemaphores,
		dispatchSubmitWaitStageFlags,
		cmdPool.getData().cmd[1],
		dispatchFinished
	};

	dispatchSubmit.setPNext(&dispatchSubmitTimelineInfo);
	core.apiBase().computeQueue().submit({ inputSubmit, dispatchSubmit});

	recordOutputDynamicCB();

	vk::TimelineSemaphoreSubmitInfo outputSubmitTimelineInfo{
		dispatchFinishedWaitVal, //wait
		++outputFinishedWaitVal //signal
	};

	std::vector<vk::CommandBuffer> outputSubmitCB = { cmdPool.getData().cmd[2], outputDynamicCB };

	vk::SubmitInfo outputSubmit{
		dispatchFinished,
		vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer),
		outputSubmitCB,
		outputFinished
	};

	outputSubmit.setPNext(&outputSubmitTimelineInfo);

	core.apiBase().computeQueue().submit(outputSubmit);
}

Compute::~Compute() {
	free();
}

}