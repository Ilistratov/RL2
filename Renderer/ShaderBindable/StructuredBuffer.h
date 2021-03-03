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
	STGUPtr srcCopyInProgress;
	std::vector<STGUPtr> srcCopyDone;

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

	void pendCopy(STGUPtr&& n_srcCopy, std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {});
	STGUPtr&& retriveUsedStg();
	Pipeline::DescriptorBinding getBinding() const override;

	void recordDynamic(vk::CommandBuffer cmd) override;
};

}