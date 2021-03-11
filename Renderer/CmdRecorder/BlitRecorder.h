#pragma once

#include "ICmdRecorder.h"
#include "ImgBarrierRecorder.h"

namespace Renderer::CmdRecorder {

struct BlitTarget {
	vk::Image img;
	vk::ImageMemoryBarrier preBlit;
	vk::ImageMemoryBarrier postBlit;
};

class BlitRecorder : public ICmdRecorder {
	ImgBarrierRecorder preBlit;
	ImgBarrierRecorder postBlit;
	vk::ImageBlit blit;
	vk::Image src;
	vk::Image dst;

public:
	BlitRecorder() = default;
	
	BlitRecorder(
		BlitTarget src,
		BlitTarget dst,
		vk::ImageBlit blit,
		vk::PipelineStageFlags renderingStage = vk::PipelineStageFlagBits::eComputeShader
	);

	void recordBlit(vk::CommandBuffer cmd);
	void record(vk::CommandBuffer cmd) override;
};

}