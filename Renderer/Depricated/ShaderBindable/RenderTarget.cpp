#include "Renderer\Core.h"
#include "RenderTarget.h"

namespace Renderer::ShaderBindable {

RenderTarget::RenderTarget(
	ResourceHandler::ImageBase::Data blitDst,
	vk::ImageMemoryBarrier dstInit,
	vk::ImageMemoryBarrier dstPreblit,
	vk::ImageMemoryBarrier dstPostBlit,
	vk::ShaderStageFlags visibleStages
) :	blitDst(blitDst),
	dstInit(dstInit),
	dstPreblit(dstPreblit),
	dstPostBlit(dstPostBlit),
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
	std::swap(dstInit, other.dstInit);
	std::swap(dstPreblit, other.dstPreblit);
	std::swap(dstPostBlit, other.dstPostBlit);
	std::swap(imgView, other.imgView);
	std::swap(visibleStages, other.visibleStages);
	renderTarget.swap(other.renderTarget);
}

void RenderTarget::free() {
	blitDst = ResourceHandler::ImageBase::Data{};
	dstInit = vk::ImageMemoryBarrier{};
	dstPreblit = vk::ImageMemoryBarrier{};
	dstPostBlit = vk::ImageMemoryBarrier{};
	visibleStages = {};
	core.device().destroyImageView(imgView);
	imgView = vk::ImageView{};
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

void RenderTarget::recordInit(vk::CommandBuffer cmd) {
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTopOfPipe,
		vk::PipelineStageFlagBits::eComputeShader,
		{},
		{},
		{},
		{ renderTarget.genInitBarrier(), dstInit }
	);
}

void RenderTarget::recordRegular(vk::CommandBuffer cmd) {
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer,
		{},
		{},
		{},
		{ renderTarget.genPreBlitBarrier(), dstPreblit }
	);
	
	renderTarget.recordBlit(cmd, blitDst);
	
	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eComputeShader,
		{},
		{},
		{},
		{ renderTarget.genPreRenderBarrier(), dstPostBlit }
	);
}

RenderTarget::~RenderTarget() {
	free();
}

}

