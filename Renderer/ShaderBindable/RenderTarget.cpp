#include "RenderTarget.h"

namespace Renderer::ShaderBindable {

RenderTarget::RenderTarget(ResourceHandler::ImageBase::Data blitDst, vk::ShaderStageFlags visibleStages) :
	blitDst(blitDst),
	renderTarget(blitDst.ext, blitDst.fmt),
	imgView(renderTarget.createImageView()),
	visibleStages(visibleStages) {}

RenderTarget::RenderTarget(RenderTarget&& other) {
	swap(other);
}

void RenderTarget::operator=(RenderTarget&& other) {
	swap(other);
	other.free();
}

void RenderTarget::swap(RenderTarget& other) {
	std::swap(blitDst, other.blitDst);
	renderTarget.swap(other.renderTarget);
}

void RenderTarget::free() {
	blitDst = ResourceHandler::ImageBase::Data{};
	renderTarget.free();
}

Pipeline::DescriptorBinding RenderTarget::getBinding() const {
	return Pipeline::DescriptorBinding{
		{},
		{vk::DescriptorImageInfo{
			{},
			imgView,
			vk::ImageLayout::eGeneral
		}},
		visibleStages,
		vk::DescriptorType::eStorageImage
	};
}

void RenderTarget::recordRegular(vk::CommandBuffer cmd) {
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer,
		{},
		{},
		{},
		renderTarget.genPreBlitBarrier()
	);
	
	renderTarget.recordBlit(cmd, blitDst);
	
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eComputeShader,
		{},
		{},
		{},
		renderTarget.genPreRenderBarrier()
	);
}

}

