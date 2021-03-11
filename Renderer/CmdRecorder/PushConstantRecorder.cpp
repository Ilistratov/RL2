#include "PushConstantRecorder.h"

namespace Renderer::CmdRecorder {

PushConstantRecorder::PushConstantRecorder(
	const std::vector<ResourceHandler::IPushConstant*>& pushConstants,
	vk::PipelineLayout pplnLayt
) : pushConstants(pushConstants), pplnLayt(pplnLayt) {}

void PushConstantRecorder::recordPush(vk::CommandBuffer cmd) {
	for (auto pc : pushConstants) {
		pc->recordPush(cmd, pplnLayt);
	}
}

void PushConstantRecorder::record(vk::CommandBuffer cmd) {
	recordPush(cmd);
}

}