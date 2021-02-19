#include "IShaderBindable.h"

namespace Renderer::ShaderHandler {

IShaderBindable::Barriers IShaderBindable::collectPreLoadDataBarriers() { return {}; }
IShaderBindable::Barriers IShaderBindable::collectPreDispatchBarriers() { return {}; }
IShaderBindable::Barriers IShaderBindable::collectPreTransferResultBarriers() { return {}; }

void IShaderBindable::recordInit(vk::CommandBuffer) {}

void IShaderBindable::recordLoadData(vk::CommandBuffer) {}
void IShaderBindable::recordLoadDataDynamic(vk::CommandBuffer) {}

void IShaderBindable::recordTransferResult(vk::CommandBuffer) {}
void IShaderBindable::recordTransferResultDynamic(vk::CommandBuffer) {}

std::vector<Pipeline::IDescriptorBindable*> IShaderBindable::collectSetBindables() { return {}; }
std::vector<vk::PushConstantRange> IShaderBindable::collectPushConstants() { return {}; }

void IShaderBindable::Barriers::record(
	vk::CommandBuffer& cmd,
	vk::PipelineStageFlags srcStage,
	vk::PipelineStageFlags dstStage
) {
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eComputeShader,
		{},
		{},
		buff,
		img
	);
}

}