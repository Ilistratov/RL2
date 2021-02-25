#pragma once

#include "Renderer\PipelineHandler\DescriptorBinding.h"
#include "Renderer\ResourceHandler\RenderTarget.h"
#include "IShaderControlable.h"

namespace Renderer::ShaderBindable {

class RenderTarget : public Pipeline::IDescriptorBindable, public IShaderOutput {
	ResourceHandler::ImageBase::Data blitDst;
	ResourceHandler::RenderTarget renderTarget;
	vk::ImageView imgView;
	vk::ShaderStageFlags visibleStages;

public:
	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator =(const RenderTarget&) = delete;

	//only img, extent and format fields of blitDst are needed,
	//the rest can be 0 or VK_NULL_HANDLE or default constructed or whatever...
	RenderTarget(ResourceHandler::ImageBase::Data blitDst, vk::ShaderStageFlags visibleStages);

	RenderTarget(RenderTarget&& other);
	void operator =(RenderTarget&& other);

	void swap(RenderTarget& other);
	void free();

	Pipeline::DescriptorBinding getBinding() const override;
	void recordRegular(vk::CommandBuffer cmd) override;
};

}