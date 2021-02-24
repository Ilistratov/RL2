#include "BindableStructuredBuffer.h"

namespace Renderer::ShaderBindable {

BindableStructuredBuffer::BindableStructuredBuffer(
	ResourceHandler::StructuredBuffer&& buff,
	vk::ShaderStageFlags visibleStages
) : buff(std::move(buff)), visibleStages(visibleStages), srcCopy(nullptr) {}

BindableStructuredBuffer::BindableStructuredBuffer(BindableStructuredBuffer&& other) {
	swap(other);
}

void BindableStructuredBuffer::operator=(BindableStructuredBuffer&& other) {
	swap(other);
	other.free();
}

void BindableStructuredBuffer::swap(BindableStructuredBuffer& other) {
	std::scoped_lock lock(srcCopyAccess, other.srcCopyAccess);
	buff.swap(other.buff);
	std::swap(visibleStages, other.visibleStages);
	srcCopy.swap(other.srcCopy);
	copyRegions.swap(other.copyRegions);
	std::swap(isSrcPendingCopy, other.isSrcPendingCopy);
}

void BindableStructuredBuffer::free() {
	buff.free();
	visibleStages = vk::ShaderStageFlags{};
	
	srcCopy.reset();
	
	copyRegions.clear();
	copyRegions.shrink_to_fit();
	
	isSrcPendingCopy = false;
}

BindableStructuredBuffer::STGUPtr&& BindableStructuredBuffer::swapCopySrcStgBuff(
	STGUPtr&& n_srcCopy,
	std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {}
) {
	std::scoped_lock lock(srcCopyAccess);
	std::swap(srcCopy, n_srcCopy);
	isSrcPendingCopy = true;
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
	
	if (isSrcPendingCopy) {
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
		
		isSrcPendingCopy = false;
	}
}

}