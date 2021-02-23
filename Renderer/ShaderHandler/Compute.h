#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "Renderer\ResourceHandler\CommandPool.h"
#include "Renderer\ShaderBindable\IShaderControlable.h"

namespace Renderer::ShaderHandler {

class Compute {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	
	ResourceHandler::CommandPool cmdPool;
	ShaderBindable::IShaderControlable* ctr = nullptr;

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
		ShaderBindable::IShaderControlable* ctr,
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