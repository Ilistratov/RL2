#include "BindableStructuredBuffer.h"

namespace Renderer::ShaderBindable {

BindableStructuredBuffer::BindableStructuredBuffer(
	ResourceHandler::StructuredBuffer&& buff,
	vk::ShaderStageFlags visibleStages
) : buff(std::move(buff)), visibleStages(visibleStages), srcCopy(nullptr) {}

BindableStructuredBuffer::STGUPtr&& BindableStructuredBuffer::swapCopySrcStgBuff(
	STGUPtr&& n_srcCopy,
	std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {}
) {
	std::scoped_lock lock(srcCopyAccess);
	std::swap(srcCopy, n_srcCopy);
	srcCopyPending = true;
	return std::move(srcCopy);
}

Pipeline::DescriptorBinding BindableStructuredBuffer::getBinding() const {
	return Pipeline::DescriptorBinding{
		{ vk::DescriptorBufferInfo{ } },
		{},
		visibleStages,
		vk::DescriptorType::eStorageBuffer
	};
}

void BindableStructuredBuffer::recordLoadDataDynamic(vk::CommandBuffer cmd) {
	std::scoped_lock lock(srcCopyAccess);
	
	if (srcCopyPending) {
		srcCopy->recordCopyTo(cmd, buff, buff.toBufferCopy(copyRegions));
		cmd.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eComputeShader,
			{},
			{},
			buff.genUploadBarrier(),
			{}
		);
		
		copyRegions.clear();
		copyRegions.shrink_to_fit();
		
		srcCopyPending = false;
	}
}

}