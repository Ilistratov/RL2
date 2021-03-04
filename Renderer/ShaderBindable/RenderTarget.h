#pragma once

#include "Renderer\PipelineHandler\DescriptorBinding.h"
#include "Renderer\ResourceHandler\RenderTarget.h"
#include "Renderer\ICmdRecorder.h"

namespace Renderer::ShaderBindable {

class RenderTarget : public Pipeline::IDescriptorBindable, public ICmdRecorder {
	ResourceHandler::ImageBase::Data blitDst;
	vk::ImageMemoryBarrier dstInit;
	vk::ImageMemoryBarrier dstPreblit;
	vk::ImageMemoryBarrier dstPostBlit;
	
	ResourceHandler::RenderTarget renderTarget;
	vk::ImageView imgView;
	vk::ShaderStageFlags visibleStages;

public:
	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator =(const RenderTarget&) = delete;

	//only img, extent and format fields of blitDst are needed,
	//the rest can be 0 or VK_NULL_HANDLE or default constructed or whatever...
	RenderTarget(
		ResourceHandler::ImageBase::Data blitDst,
		vk::ImageMemoryBarrier dstInit,
		vk::ImageMemoryBarrier dstPreblit,
		vk::ImageMemoryBarrier dstPostBlit,
		vk::ShaderStageFlags visibleStages = vk::ShaderStageFlagBits::eAll
	);

	RenderTarget(RenderTarget&& other);
	void operator =(RenderTarget&& other);

	void swap(RenderTarget& other);
	void free();

	Pipeline::DescriptorBinding getBinding() const override;
	void recordInit(vk::CommandBuffer cmd) override;
	void recordRegular(vk::CommandBuffer cmd) override;

	~RenderTarget();
};

}