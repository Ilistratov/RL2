#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Renderer\DescriptorHandler\Pool.h"

namespace Renderer::Pipeline {

class Compute {
	vk::Pipeline ppln;
	vk::PipelineLayout layt;
	DescriptorHandler::Pool dPool;

public:
	Compute(Compute&) = delete;
	Compute& operator =(Compute&) = delete;

	Compute() = default;
	Compute(
		const DescriptorHandler::Layout& dLayout,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain
	);

	void swap(Compute& other);
	void free();

	Compute(Compute&& other);
	void operator =(Compute&& other);

	~Compute();

	void bind(vk::CommandBuffer& cmd);

	DescriptorHandler::Pool& getDPool();
};

}