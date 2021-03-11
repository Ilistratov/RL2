#include "BlitRecorder.h"

namespace Renderer::CmdRecorder {

BlitRecorder::BlitRecorder(
	BlitTarget src,
	BlitTarget dst,
	vk::ImageBlit blit,
	vk::PipelineStageFlags renderingStage
) : preBlit(
		renderingStage,
		vk::PipelineStageFlagBits::eTransfer,
		{ src.preBlit, dst.preBlit }
	),
	blit(blit),
	postBlit(
		vk::PipelineStageFlagBits::eTransfer,
		renderingStage,
		{ src.postBlit, dst.postBlit }
	),
	src(src.img),
	dst(dst.img) {}

void BlitRecorder::recordBlit(vk::CommandBuffer cmd) {
	preBlit.recordBarriers(cmd);
	cmd.blitImage(
		src,
		vk::ImageLayout::eTransferSrcOptimal,
		dst,
		vk::ImageLayout::eTransferDstOptimal,
		blit,
		vk::Filter::eLinear
	);
	
	postBlit.recordBarriers(cmd);
}

void BlitRecorder::record(vk::CommandBuffer cmd) {
	recordBlit(cmd);
}

}