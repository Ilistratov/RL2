#include "ICmdRecorder.h"

namespace Renderer::CmdRecorder {

void ICmdRecorder::record(vk::CommandBuffer cmd) {}

CompositCmdRecorder::CompositCmdRecorder(const std::vector<ICmdRecorder*>& cmdRecorders) : cmdRecorders(cmdRecorders) {}

void CompositCmdRecorder::record(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->record(cmd);
	}
}

}