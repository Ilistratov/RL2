#include "IShaderControlable.h"

namespace Renderer::ShaderBindable {

void IShaderControlable::recordInit(vk::CommandBuffer) {}

void IShaderControlable::recordLoadData(vk::CommandBuffer) {}
void IShaderControlable::recordLoadDataDynamic(vk::CommandBuffer) {}

void IShaderControlable::recordTransferResult(vk::CommandBuffer) {}
void IShaderControlable::recordTransferResultDynamic(vk::CommandBuffer) {}

}

}