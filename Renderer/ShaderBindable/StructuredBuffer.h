#pragma once

#include <memory>
#include <mutex>

#include "Renderer\PipelineHandler\DescriptorBinding.h"
#include "Renderer\ResourceHandler\StructuredBuffer.h"
#include "Renderer\ResourceHandler\StagingBuffer.h"
#include "Renderer\ICmdRecorder.h"

namespace Renderer::ShaderBindable {

class StructuredBuffer : public Pipeline::IDescriptorBindable, public ICmdRecorder {
	ResourceHandler::StructuredBuffer buff;
	vk::ShaderStageFlags visibleStages;

	std::mutex srcCopyAccess;
	using STGUPtr = std::unique_ptr<ResourceHandler::StagingBuffer>;
	STGUPtr srcCopy;
	std::vector<ResourceHandler::StructuredBufferCopy> copyRegions;
	bool isSrcPendingCopy = false;

public:
	//TODO
	StructuredBuffer(const StructuredBuffer&) = delete;
	StructuredBuffer& operator =(const StructuredBuffer&) = delete;

	StructuredBuffer(
		ResourceHandler::StructuredBuffer&& buff,
		vk::ShaderStageFlags visibleStages = vk::ShaderStageFlagBits::eAll
	);

	StructuredBuffer(StructuredBuffer&& other);
	void operator =(StructuredBuffer&& other);

	void swap(StructuredBuffer& other);
	
	//not thread safe
	void free();

	//if there was no srcCopy assigned before, returns nullptr
	//otherwise, will return ownership of previously assigned srcCopy.
	STGUPtr&& swapCopySrcStgBuff(STGUPtr&& n_srcCopy, std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {});
	Pipeline::DescriptorBinding getBinding() const override;

	void recordDynamic(vk::CommandBuffer cmd) override;
};

}