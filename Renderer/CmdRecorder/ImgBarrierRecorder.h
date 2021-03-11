#pragma once

#include "ICmdRecorder.h"

namespace Renderer::CmdRecorder {

class ImgBarrierRecorder : public ICmdRecorder {
	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;
	std::vector<vk::ImageMemoryBarrier> barriers;
public:
	ImgBarrierRecorder() = default;
	
	ImgBarrierRecorder(
		vk::PipelineStageFlags srcStage,
		vk::PipelineStageFlags dstStage,
		std::vector<vk::ImageMemoryBarrier> barriers
	);

	//in order to avoid virtual function call where we don't need
	//to call a virtual function
	void recordBarriers(vk::CommandBuffer cmd);
	void record(vk::CommandBuffer cmd) override;
};

}