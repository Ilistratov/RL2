#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "Renderer\ResourceHandler\CommandPool.h"
#include "Renderer\ShaderBindable\IShaderBindable.h"

namespace Renderer::ShaderHandler {

class Compute {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	
	ResourceHandler::CommandPool cmdPool;
	ShaderBindable::IShaderBindable* bnd = nullptr;

	std::tuple<int, int, int> dispatchDim = { 0, 0, 0 };

	vk::CommandBuffer loadDataDynamicCB;
	vk::CommandBuffer transferResultDynamicCB;

	vk::Fence submissionFinished;
	vk::Semaphore loadDataFinished;
	vk::Semaphore dispatchFinished;

	vk::CommandBuffer recordInit();

	void waitSubmissionFin();
	void resetSubmissionFin();

	void recordLoadData();
	void recordDispatch();
	void recordTransferResult();

public:
	Compute(const Compute& other) = delete;
	Compute& operator =(const Compute& other) = delete;

	Compute() = default;

	Compute(
		ShaderBindable::IShaderBindable* bnd,
		const std::string& shaderFilePath,
		const std::string& shaderMain,
		std::tuple<int, int, int> dispatchDim
	);

	Compute(Compute&& other);
	void operator =(Compute&& other);

	void swap(Compute& other);
	void free();

	void dispatch();

	~Compute();
};

}