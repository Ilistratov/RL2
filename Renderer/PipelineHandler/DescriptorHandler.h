#pragma once

#include <vector>

#include "Renderer\Core.h"
#include "DPoolLayoutFactory.h"

namespace Renderer::Pipeline {

class DescriptorHandler {
	std::vector<uint64_t> dPoolDataId;
public:
	//dPoolDataReservedId - reserved index of the first
	//DescriptorPoolData in the core.dPools corresponding to
	//this handler. If provided, ensure that
	//there is at least as much elements
	//as dPools in bindings provided by factory(factory.genLayoutBindings().size())
	DescriptorHandler(std::vector<DPoolLayoutFactory>& factory, uint64_t dPoolDataReservedId = UINT64_MAX);
};

}
