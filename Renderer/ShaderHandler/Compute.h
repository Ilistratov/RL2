#pragma once

#include "Renderer\PipelineHandler\Compute.h"
#include "Renderer\ResourceHandler\CommandPool.h"
#include "IShaderBindable.h"

namespace Renderer::ShaderHandler {

class Compute {
	Pipeline::DPoolHandler dPool;
	Pipeline::Compute pipeline;
	ResourceHandler::CommandPool cmdPool;
	IShaderBindable* bnd;

	vk::Fence recordAndSubmitInitCB();

public:
	Compute(const Compute& other) = delete;
	Compute& operator =(const Compute& other) = delete;

	Compute() = default;

	Compute(
		IShaderBindable* bnd,
		const std::string& shaderFilePath,
		const std::string& shaderMain
	);
};

}