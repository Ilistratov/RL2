#include "IShaderBindable.h"

namespace Renderer::ShaderHandler {

IShaderBindable::Barriers IShaderBindable::collectPreLoadDataBarriers(BarrierStages stage) { return {}; }
IShaderBindable::Barriers IShaderBindable::collectPreDispatchBarriers(BarrierStages stage) { return {}; }
IShaderBindable::Barriers IShaderBindable::collectPreTransferResultBarriers(BarrierStages stage) { return {}; }

void IShaderBindable::recordInit(vk::CommandBuffer cb) {}

void IShaderBindable::recordLoadData(vk::CommandBuffer cb) {}
void IShaderBindable::recordLoadDataDynamic(vk::CommandBuffer cb) {}

void IShaderBindable::recordTransferResult(vk::CommandBuffer cb) {}
void IShaderBindable::recordTransferResultDynamic(vk::CommandBuffer cb) {}

std::vector<Pipeline::IDescriptorBindable*> IShaderBindable::collectSetBindables() { return {}; }
std::vector<vk::PushConstantRange> IShaderBindable::collectPushConstants() { return {}; }

}