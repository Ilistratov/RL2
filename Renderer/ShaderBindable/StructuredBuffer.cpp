#include "StructuredBuffer.h"

namespace Renderer::ShaderBindable {

StructuredBuffer::StructuredBuffer(
	ResourceHandler::StructuredBuffer&& buff,
	vk::ShaderStageFlags visibleStages
) : buff(std::move(buff)), visibleStages(visibleStages), srcCopy(nullptr) {}

StructuredBuffer::StructuredBuffer(StructuredBuffer&& other) {
	swap(other);
}

void StructuredBuffer::operator=(StructuredBuffer&& other) {
	swap(other);
	other.free();
}

void StructuredBuffer::swap(StructuredBuffer& other) {
	std::scoped_lock lock(srcCopyAccess, other.srcCopyAccess);
	buff.swap(other.buff);
	std::swap(visibleStages, other.visibleStages);
	srcCopy.swap(other.srcCopy);
	copyRegions.swap(other.copyRegions);
	std::swap(isSrcPendingCopy, other.isSrcPendingCopy);
}

void StructuredBuffer::free() {
	buff.free();
	visibleStages = vk::ShaderStageFlags{};
	
	srcCopy.reset();
	
	copyRegions.clear();
	copyRegions.shrink_to_fit();
	
	isSrcPendingCopy = false;
}

StructuredBuffer::STGUPtr&& StructuredBuffer::swapCopySrcStgBuff(
	STGUPtr&& n_srcCopy,
	std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {}
) {
	std::scoped_lock lock(srcCopyAccess);
	std::swap(srcCopy, n_srcCopy);
	isSrcPendingCopy = true;
	return std::move(srcCopy);
}

Pipeline::DescriptorBinding StructuredBuffer::getBinding() const {
	return Pipeline::DescriptorBinding{
		{ vk::DescriptorBufferInfo{ } },
		{},
		visibleStages,
		vk::DescriptorType::eStorageBuffer
	};
}

void StructuredBuffer::recordDynamic(vk::CommandBuffer cmd) {
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