#include "CmdExecutor.h"

namespace Renderer {

void CmdExecutor::recordRegular(uint64_t stageInd) {
	auto& cmd = cmdPool.getData().cmd[stageInd];
	cmd.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eSimultaneousUse
		}
	);
	stages[stageInd].recorder->recordRegular(cmd);
	cmd.end();
}

void CmdExecutor::recordDynamic(uint64_t stageInd) {
	core.device().waitSemaphores(
		vk::SemaphoreWaitInfo{
			{},
			stageFinished,
			stageFinishedWaitVal
		},
		UINT64_MAX
	);

	auto& cmd = cmdDynamic[stageInd];

	cmd.reset();
	cmd.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);
	stages[stageInd].recorder->recordDynamic(cmd);
	cmd.end();
}

void CmdExecutor::submitStage(uint64_t stageInd) {
	recordDynamic(stageInd);
	
	std::vector<vk::Semaphore> wait;
	std::vector<vk::PipelineStageFlags> waitStages;
	wait.reserve(stages[stageInd].internalWait.size() + stages[stageInd].externalWait.size());
	wait.reserve(stages[stageInd].internalWait.size() + stages[stageInd].externalWait.size());
	
	std::vector<uint64_t> internalWaitValues;
	internalWaitValues.reserve(stages[stageInd].internalWait.size());

	for (auto waitInd : stages[stageInd].internalWait) {
		//if waitInd < stageInd, we are waiting on
		//stage, that was submited for execution on this iteration
		//(as it conveniently turns out that we
		//already submited this stages and incremented their
		//stageFinishedWaitVal)
		//if waitInd > stageInd we are waiting on
		//stage, thar was submited for execution on previous iteration
		//and it just turns out, that their stageFinishedWaitVal is
		//also the one we need
		wait.push_back(stageFinished[stageInd]);
		internalWaitValues.push_back(stageFinishedWaitVal[waitInd]);
	}
	wait.insert(wait.end(), stages[stageInd].externalWait.begin(), stages[stageInd].externalWait.end());
	
	waitStages.insert(waitStages.end(), stages[stageInd].internalWaitStage.begin(), stages[stageInd].internalWaitStage.end());
	waitStages.insert(waitStages.end(), stages[stageInd].externalWaitStage.begin(), stages[stageInd].externalWaitStage.end());

	vk::TimelineSemaphoreSubmitInfo internalWaitInfo{
		internalWaitValues,
		++stageFinishedWaitVal[stageInd]
	};

	std::vector<vk::Semaphore> signal;
	signal.reserve(1 + stages[stageInd].externalSignal.size());
	signal.push_back(stageFinished[stageInd]);
	signal.insert(signal.end(), stages[stageInd].externalSignal.begin(), stages[stageInd].externalSignal.end());

	std::vector<vk::CommandBuffer> cmd = { cmdPool.getData().cmd[stageInd], cmdDynamic[stageInd] };

	vk::SubmitInfo submit{
		wait,
		waitStages,
		cmd,
		signal
	};
	submit.setPNext(&internalWaitInfo);

	core.apiBase().computeQueue().submit(submit);
}

CmdExecutor::CmdExecutor(
	std::vector<ExecutionStageDescription>&& stageDescriptions
) : stages(stageDescriptions),
	cmdPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute)),
	cmdDynamic(stages.size()),
	stageFinished(stages.size()),
	stageFinishedWaitVal(stages.size()) {
	cmdPool.reserve(stages.size());

	vk::CommandBuffer cmdInit = cmdPool.reserveOneTimeSubmit();

	for (uint64_t i = 0; i < stages.size(); i++) {
		stages[i].recorder->recordInit(cmdInit);
	}

	vk::Fence initFinished = core.device().createFence({});

	core.apiBase().computeQueue().submit(
		vk::SubmitInfo{
			{},
			{},
			cmdInit,
			{}
		},
		initFinished
	);

	core.device().waitForFences(initFinished, true, UINT64_MAX);
	cmdPool.freeOneTimeSubmit({ cmdInit });

	for (uint64_t i = 0; i < stages.size(); i++) {
		recordRegular(i);
	}

	for (uint64_t i = 0; i < stages.size(); i++) {
		vk::SemaphoreTypeCreateInfo stci{
			vk::SemaphoreType::eTimeline,
			0
		};

		vk::SemaphoreCreateInfo ci;
		ci.setPNext(&stci);

		stageFinished[i] = core.device().createSemaphore(ci);
	}

	for (auto& cmd : cmdDynamic) {
		cmd = cmdPool.reserveOneTimeSubmit();
	}
}

CmdExecutor::CmdExecutor(CmdExecutor&& other) {
	swap(other);
}

void CmdExecutor::operator=(CmdExecutor&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

void CmdExecutor::swap(CmdExecutor& other) {
	stages.swap(other.stages);
	
	cmdPool.swap(other.cmdPool);
	cmdDynamic.swap(other.cmdDynamic);

	stageFinished.swap(other.stageFinished);
	stageFinishedWaitVal.swap(other.stageFinishedWaitVal);
}

void CmdExecutor::free() {
	stages.clear();
	stages.shrink_to_fit();

	cmdPool.freeOneTimeSubmit(cmdDynamic);
	
	cmdDynamic.clear();
	cmdDynamic.shrink_to_fit();

	cmdPool.free();

	for (auto& el : stageFinished) {
		core.device().destroySemaphore(el);
	}

	stageFinished.clear();
	stageFinished.shrink_to_fit();

	stageFinishedWaitVal.clear();
	stageFinishedWaitVal.shrink_to_fit();
}

void CmdExecutor::submit() {
	for (uint64_t i = 0; i < stages.size(); i++) {
		submitStage(i);
	}
}

CmdExecutor::~CmdExecutor() {
	free();
}

}