#pragma once

#include "Renderer\ResourceHandler\PushConstant.h"
#include "ICmdRecorder.h"

namespace Renderer::CmdRecorder {

class PushConstantRecorder :public ICmdRecorder {
	std::vector<ResourceHandler::IPushConstant*> pushConstants;
	vk::PipelineLayout pplnLayt;

public:
	PushConstantRecorder() = default;
	PushConstantRecorder(
		const std::vector<ResourceHandler::IPushConstant*>& pushConstants,
		vk::PipelineLayout pplnLayt
	);

	void recordPush(vk::CommandBuffer cmd);
	void record(vk::CommandBuffer cmd) override;
};

}