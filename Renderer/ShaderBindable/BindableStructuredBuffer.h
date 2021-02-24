#pragma once

#include <memory>
#include <mutex>

#include "Renderer\PipelineHandler\DescriptorBinding.h"
#include "Renderer\ResourceHandler\StructuredBuffer.h"
#include "Renderer\ResourceHandler\StagingBuffer.h"
#include "Renderer\ShaderBindable\IShaderControlable.h"

namespace Renderer::ShaderBindable {

class BindableStructuredBuffer : public Pipeline::IDescriptorBindable, public ShaderBindable::IShaderControlable {
	ResourceHandler::StructuredBuffer buff;
	vk::ShaderStageFlags visibleStages;

	std::mutex srcCopyAccess;
	using STGUPtr = std::unique_ptr<ResourceHandler::StagingBuffer>;
	STGUPtr srcCopy;
	std::vector<ResourceHandler::StructuredBufferCopy> copyRegions;
	bool isSrcPendingCopy = false;

public:
	//TODO
	BindableStructuredBuffer(const BindableStructuredBuffer&) = delete;
	BindableStructuredBuffer& operator =(const BindableStructuredBuffer&) = delete;

	BindableStructuredBuffer(ResourceHandler::StructuredBuffer&& buff, vk::ShaderStageFlags visibleStages = vk::ShaderStageFlagBits::eAll);

	BindableStructuredBuffer(BindableStructuredBuffer&& other);
	void operator =(BindableStructuredBuffer&& other);

	void swap(BindableStructuredBuffer& other);
	
	//not thread safe
	void free();

	//if there was no srcCopy assigned before, returns nullptr
	//otherwise, will return ownership of previously assigned srcCopy.
	STGUPtr&& swapCopySrcStgBuff(STGUPtr&& n_srcCopy, std::vector<ResourceHandler::StructuredBufferCopy>&& n_copyRegions = {});
	Pipeline::DescriptorBinding getBinding() const;

	void recordLoadDataDynamic(vk::CommandBuffer cmd) override;
};

}