#pragma once

#include <vector>

#include "Renderer\Core.h"
#include "DSetlLayoutFactory.h"

namespace Renderer::Pipeline {

class DPoolHandler {
	uint64_t dPoolDataId;
public:
	DPoolHandler() = default;
	
	DPoolHandler(std::vector<DSetLayoutFactory>& factory, uint64_t dPoolDataReservedId = UINT64_MAX);
	
	DataComponent::DescriptorPoolData& getData();
	const DataComponent::DescriptorPoolData& getData() const;
};

}
