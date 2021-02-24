#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "Renderer\ResourceHandler\CommandPool.h"
#include "Renderer\ShaderBindable\IShaderControlable.h"

namespace Renderer::ShaderHandler {

//access must by
//externally synchronized
class Compute {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	
	ResourceHandler::CommandPool cmdPool;
	ShaderBindable::IShaderInput* inputCtr = nullptr;
	ShaderBindable::IShaderOutput* outputCtr = nullptr;

	std::tuple<int, int, int> dispatchDim = { 0, 0, 0 };

	vk::CommandBuffer inputDynamicCB;
	vk::CommandBuffer outputDynamicCB;

	//timeline semaphores are used, not binary
	vk::Semaphore inputFinished;
	uint64_t inputFinishedWaitVal = 0;
	vk::Semaphore dispatchFinished;
	uint64_t dispatchFinishedWaitVal = 0;
	vk::Semaphore outputFinished;
	uint64_t outputFinishedWaitVal = 0;

	vk::CommandBuffer recordInit();

	void recordInputRegularCB();
	void recordDispatchCB();
	void recordOutputRegularCB();

	void recordInputDynamicCB();
	void recordOutputDynamicCB();
public:
	Compute(const Compute& other) = delete;
	Compute& operator =(const Compute& other) = delete;

	Compute() = default;

	Compute(
		ShaderBindable::IShaderInput* inputCtr,
		ShaderBindable::IShaderOutput* outputCtr,
		const std::vector<Pipeline::SetBindable>& bnd,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain,
		std::tuple<int, int, int> dispatchDim
	);

	Compute(Compute&& other);
	void operator =(Compute&& other);

	void dispatch();

	void swap(Compute& other);
	void free();

	~Compute();
};

}