#pragma once

#include "ICmdRecorder.h"

namespace Renderer::CmdRecorder {

struct BlitTarget {
	vk::Image img;
	vk::Extent2D ext;
	vk::ImageMemoryBarrier preBlit;
	vk::ImageMemoryBarrier postBlit;
};

class BlitRecorder : public ICmdRecorder {
	BlitTarget src;
	BlitTarget dst;
	vk::ImageBlit blit;
	vk::PipelineStageFlags renderingStage;

public:
	BlitRecorder() = default;
	
	BlitRecorder(
		BlitTarget src,
		BlitTarget dst,
		vk::ImageBlit blit,
		vk::PipelineStageFlags renderingStage = vk::PipelineStageFlagBits::eComputeShader
	);

	void recordStatic(vk::CommandBuffer cmd) override;
};

}