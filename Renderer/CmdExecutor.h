#pragma once

#include "ICmdRecorder.h"
#include "ResourceHandler\CommandPool.h"

namespace Renderer {

struct ExecutionStageDescription {
	ICmdRecorder* recorder;
	std::vector<uint64_t> internalWait = {};
	std::vector<vk::PipelineStageFlags> internalWaitStage = {};
	std::vector<vk::Semaphore> externalSignal = {};
	std::vector<vk::Semaphore> externalWait = {};
	std::vector<vk::PipelineStageFlags> externalWaitStage = {};
};

//currently works only for queue with compute support, which is okay for now
//since my corrent gpu has queue with support of all three operation types
class CmdExecutor {
	std::vector<ExecutionStageDescription> stages;
	
	ResourceHandler::CommandPool cmdPool;
	std::vector<vk::CommandBuffer> cmdDynamic;
	
	std::vector<vk::Semaphore> stageFinished;
	std::vector<uint64_t> stageFinishedWaitVal;

	void recordRegular(uint64_t stageInd);
	void recordDynamic(uint64_t stageInd);
	void submitStage(uint64_t stageInd);

public:
	CmdExecutor(const CmdExecutor&) = delete;
	CmdExecutor& operator =(const CmdExecutor&) = delete;
	
	CmdExecutor() = default;
	CmdExecutor(std::vector<ExecutionStageDescription>&& stageDescriptions);

	CmdExecutor(CmdExecutor&& other);
	void operator =(CmdExecutor&& other);

	void swap(CmdExecutor& other);
	void free();

	void submit();

	~CmdExecutor();
};

}