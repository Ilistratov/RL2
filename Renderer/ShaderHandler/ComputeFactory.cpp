#include "ComputeFactory.h"

namespace Renderer::ShaderHandler {

ComputeFactory::ComputeFactory(
	uint64_t dSetCount
) : layoutFactory(dSetCount) {}

}