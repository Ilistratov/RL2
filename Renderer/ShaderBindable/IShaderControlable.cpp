#include "IShaderControlable.h"

namespace Renderer::ShaderBindable {

IShaderControlable::Barriers&& IShaderControlable::collectPreLoadDataBarriers() { return {}; }
IShaderControlable::Barriers&& IShaderControlable::collectPreDispatchBarriers() { return {}; }
IShaderControlable::Barriers&& IShaderControlable::collectPreTransferResultBarriers() { return {}; }

void IShaderControlable::recordInit(vk::CommandBuffer) {}

void IShaderControlable::recordLoadData(vk::CommandBuffer) {}
void IShaderControlable::recordLoadDataDynamic(vk::CommandBuffer) {}

void IShaderControlable::recordTransferResult(vk::CommandBuffer) {}
void IShaderControlable::recordTransferResultDynamic(vk::CommandBuffer) {}

void IShaderControlable::Barriers::record(
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