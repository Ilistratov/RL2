#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

#include "DPoolHandler.h"

namespace Renderer::Pipeline {

class Compute {
public:
	struct Data {
		vk::Pipeline ppln;
		vk::PipelineLayout layt;
	};

	Compute(Compute&) = delete;
	Compute& operator =(Compute&) = delete;

	Compute() = default;
	Compute(
		const DPoolHandler& dPool,
		const std::vector<vk::PushConstantRange>& pushConstants,
		const std::string& shaderFilePath,
		const std::string& shaderMain
	);

	Compute(Compute&& other);
	void operator =(Compute&& other);

	void swap(Compute& other);
	void free();

	Data& getData();
	const Data& getData() const;

	~Compute();

private:
	Data data;
};

}