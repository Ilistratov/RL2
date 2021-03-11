#include "ImgBarrierRecorder.h"

namespace Renderer::CmdRecorder {

ImgBarrierRecorder::ImgBarrierRecorder(
	vk::PipelineStageFlags srcStage,
	vk::PipelineStageFlags dstStage,
	std::vector<vk::ImageMemoryBarrier> barriers
) : srcStage(srcStage),
	dstStage(dstStage),
	barriers(barriers) {}

void ImgBarrierRecorder::recordBarriers(vk::CommandBuffer cmd) {
	cmd.pipelineBarrier(
		srcStage,
		dstStage,
		{},
		{},
		{},
		barriers
	);
}

void ImgBarrierRecorder::record(vk::CommandBuffer cmd) {
	recordBarriers(cmd);
}

}
