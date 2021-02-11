#pragma once
#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class CommandPool {
	uint64_t commandPoolDataId = UINT64_MAX;
public:
	CommandPool() = default;
	CommandPool(uint64_t queueInd, uint64_t reservedCommandPoolDataId = UINT64_MAX);

	void reserve(uint64_t count);

	DataComponent::CommandPoolData& getData();
};

}