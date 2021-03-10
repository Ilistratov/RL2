#include "BlitRecorder.h"

namespace Renderer::CmdRecorder {

BlitRecorder::BlitRecorder(
	BlitTarget src,
	BlitTarget dst,
	vk::ImageBlit blit,
	vk::PipelineStageFlags renderingStage
) : src(src),
	dst(dst),
	blit(blit),
	renderingStage(renderingStage) {}

void BlitRecorder::recordStatic(vk::CommandBuffer cmd) {
	cmd.pipelineBarrier(
		renderingStage,
		vk::PipelineStageFlagBits::eTransfer,
		{},
		{},
		{},
		{ src.preBlit, dst.preBlit }
	);

	cmd.blitImage(
		src.img,
		vk::ImageLayout::eTransferSrcOptimal,
		dst.img,
		vk::ImageLayout::eTransferDstOptimal,
		blit,
		vk::Filter::eLinear
	);

	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		renderingStage,
		{},
		{},
		{},
		{ src.postBlit, dst.postBlit }
	);
}

}