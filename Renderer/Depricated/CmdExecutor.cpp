#include "Core.h"
#include "CmdExecutor.h"
//#include "Utill\Logger.h"
#include "Utill\UtillFunctions.h"

namespace Renderer {

void CmdExecutor::waitStage(uint64_t stageInd) {
	//GlobalLog.debugMsg("Enter: CmdExecutor::waitStage, stage: " + std::to_string(stageInd));
	
	auto res = core.device().waitSemaphores(
		vk::SemaphoreWaitInfo{
			{},
			stageFinished[stageInd],
			stageFinishedWaitVal[stageInd]
		},
		TIMEOUT_NSEC
	);

	if (res != vk::Result::eSuccess) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "failed to wait for semaphore"));
	}
	
	//GlobalLog.debugMsg("Exit: CmdExecutor::waitStage, stage: " + std::to_string(stageInd));
}

void CmdExecutor::recordStatic(uint64_t stageInd) {
	//GlobalLog.debugMsg("Enter: CmdExecutor::recordRegular, stage: " + std::to_string(stageInd));
	
	auto& cmd = cmdPool.getData().cmd[stageInd];
	cmd.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eSimultaneousUse
		}
	);
	stages[stageInd].cmdStatic->record(cmd);
	cmd.end();

	//GlobalLog.debugMsg("Exit: CmdExecutor::recordRegular, stage: " + std::to_string(stageInd));
}

void CmdExecutor::recordDynamic(uint64_t stageInd) {
	//GlobalLog.debugMsg("Enter: CmdExecutor::recordDynamic, stage: " + std::to_string(stageInd));
	
	waitStage(stageInd);

	auto& cmd = cmdDynamic[stageInd];

	cmd.reset();
	cmd.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);
	stages[stageInd].cmdDynamic->record(cmd);
	cmd.end();

	//GlobalLog.debugMsg("Exit: CmdExecutor::recordDynamic, stage: " + std::to_string(stageInd));
}

void CmdExecutor::submitStage(uint64_t stageInd) {
	//GlobalLog.debugMsg("Enter: CmdExecutor::submitStage, stge: " + std::to_string(stageInd));
	
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
		wait.push_back(stageFinished[waitInd]);
		internalWaitValues.push_back(stageFinishedWaitVal[waitInd]);
	}
	wait.insert(wait.end(), stages[stageInd].externalWait.begin(), stages[stageInd].externalWait.end());
	internalWaitValues.resize(stages[stageInd].internalWait.size() + stages[stageInd].externalWait.size());
	
	waitStages.insert(waitStages.end(), stages[stageInd].internalWaitStage.begin(), stages[stageInd].internalWaitStage.end());
	waitStages.insert(waitStages.end(), stages[stageInd].externalWaitStage.begin(), stages[stageInd].externalWaitStage.end());

	std::vector<uint64_t> signalValues(1 + stages[stageInd].externalSignal.size());
	signalValues[0] = ++stageFinishedWaitVal[stageInd];

	vk::TimelineSemaphoreSubmitInfo internalWaitInfo{
		internalWaitValues,
		signalValues
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

	//GlobalLog.debugMsg("Exit: CmdExecutor::submitStage, stge: " + std::to_string(stageInd));
}

CmdExecutor::CmdExecutor(
	std::vector<CmdRecorder::ICmdRecorder*> initRecorders,
	std::vector<ExecutionStageDescription>&& stageDescriptions
) : stages(stageDescriptions),
	cmdPool(core.apiBase().queueInd(vk::QueueFlagBits::eCompute)),
	cmdDynamic(stages.size()),
	stageFinished(stages.size()),
	stageFinishedWaitVal(stages.size()) {
	cmdPool.reserve(stages.size());

	vk::CommandBuffer cmdInit = cmdPool.reserveOneTimeSubmit();
	
	cmdInit.begin(
		vk::CommandBufferBeginInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		}
	);

	for (auto recorder : initRecorders) {
		recorder->record(cmdInit);
	}

	cmdInit.end();
	
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

	core.device().waitForFences(initFinished, true, TIMEOUT_NSEC);
	cmdPool.freeOneTimeSubmit({ cmdInit });
	core.device().destroyFence(initFinished);

	for (uint64_t i = 0; i < stages.size(); i++) {
		recordStatic(i);
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
	//GlobalLog.debugMsg("Enter: CmdExecutor::swap");

	stages.swap(other.stages);
	
	cmdPool.swap(other.cmdPool);
	cmdDynamic.swap(other.cmdDynamic);

	stageFinished.swap(other.stageFinished);
	stageFinishedWaitVal.swap(other.stageFinishedWaitVal);

	//GlobalLog.debugMsg("Exit: CmdExecutor::swap");
}

void CmdExecutor::free() {
	//GlobalLog.debugMsg("Enter: CmdExecutor::free");
	for (uint64_t i = 0; i < stages.size(); i++) {
		waitStage(i);
	}

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

	//GlobalLog.debugMsg("Exit: CmdExecutor::free");
}

void CmdExecutor::submit() {
	//GlobalLog.debugMsg("Enter: CmdExecutor::submit");
	
	for (uint64_t i = 0; i < stages.size(); i++) {
		submitStage(i);
	}

	//GlobalLog.debugMsg("Exit: CmdExecutor::submit");
}

CmdExecutor::~CmdExecutor() {
	//GlobalLog.debugMsg("Enter: CmdExecutor::~CmdExecutor");
	
	free();

	//GlobalLog.debugMsg("Exit: CmdExecutor::~CmdExecutor");
}

}