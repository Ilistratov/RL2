#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "Renderer\ResourceHandler\CommandPool.h"
#include "Renderer\ICmdRecorder.h"
#include "Renderer\ShaderBindable\PushConstant.h"

namespace Renderer::ShaderHandler {

//access must by
//externally synchronized
class Compute : public ICmdRecorder {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	
	std::tuple<int, int, int> dispatchDim = { 0, 0, 0 };
public:
	Compute(const Compute& other) = delete;
	Compute& operator =(const Compute& other) = delete;

	Compute() = default;

	Compute(
		const std::vector<Pipeline::SetBindable>& bnd,
		ShaderBindable::PushConstantController& pcController,
		const std::string& shaderFilePath,
		const std::string& shaderMain,
		std::tuple<int, int, int> dispatchDim
	);

	Compute(Compute&& other);
	void operator =(Compute&& other);

	void recordRegular(vk::CommandBuffer cmd) override;

	void swap(Compute& other);
	void free();

	~Compute();
};

}
