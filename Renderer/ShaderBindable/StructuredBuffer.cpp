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
	srcCopyInProgress.swap(other.srcCopyInProgress);
	srcCopyDone.swap(other.srcCopyDone);

	copyRegions.swap(other.copyRegions);
}

void StructuredBuffer::free() {
	buff.free();
	visibleStages = vk::ShaderStageFlags{};
	
	srcCopy.reset();
	srcCopyInProgress.reset();
	
	srcCopyDone.clear();
	srcCopyDone.shrink_to_fit();
	
	copyRegions.clear();
	copyRegions.shrink_to_fit();
}

void StructuredBuffer::pendCopy(STGUPtr&& n_srcCopy, std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions) {
	std::scoped_lock lock(srcCopyAccess);
	std::swap(srcCopy, n_srcCopy);
}

StructuredBuffer::STGUPtr&& StructuredBuffer::retriveUsedStg() {
	std::scoped_lock lock(srcCopyAccess);

	if (srcCopyDone.empty()) {
		return nullptr;
	}

	STGUPtr res = std::move(srcCopyDone.back());
	srcCopyDone.pop_back();
	
	return std::move(res);
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
	
	if (srcCopyInProgress.get() != nullptr) {
		srcCopyDone.push_back(std::move(srcCopyInProgress));
	}

	if (srcCopy.get() != nullptr) {
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
		srcCopyInProgress = std::move(srcCopy);
	}
}

}