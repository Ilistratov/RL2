#include "ICmdRecorder.h"

namespace Renderer::CmdRecorder {

void ICmdRecorder::recordInit(vk::CommandBuffer cmd) {}

void ICmdRecorder::recordStatic(vk::CommandBuffer cmd) {}

void ICmdRecorder::recordDynamic(vk::CommandBuffer cmd) {}

CompositCmdRecorder::CompositCmdRecorder(const std::vector<ICmdRecorder*>& cmdRecorders) : cmdRecorders(cmdRecorders) {}

void CompositCmdRecorder::recordInit(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordInit(cmd);
	}
}

void CompositCmdRecorder::recordStatic(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordStatic(cmd);
	}
}

void CompositCmdRecorder::recordDynamic(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordDynamic(cmd);
	}
}

}