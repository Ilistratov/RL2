#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "Renderer\Core.h"
#include "DPoolHandler.h"

namespace Renderer::Pipeline {

class Compute {
public:
	struct Data {
		vk::Pipeline ppln;
		vk::PipelineLayout layt;
	};

	Compute() = default;
	Compute(
		const DPoolHandler& dPool,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain
	);

	Data& getData();
	const Data& getData() const;

private:
	Data data;
};

}