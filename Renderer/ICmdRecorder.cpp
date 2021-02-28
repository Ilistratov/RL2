#include "ICmdRecorder.h"

namespace Renderer {



void ICmdRecorder::recordInit(vk::CommandBuffer cmd) {}

void ICmdRecorder::recordRegular(vk::CommandBuffer cmd) {}

void ICmdRecorder::recordDynamic(vk::CommandBuffer cmd) {}

CompositCmdRecorder::CompositCmdRecorder(const std::vector<ICmdRecorder*>& cmdRecorders) : cmdRecorders(cmdRecorders) {}

void CompositCmdRecorder::recordInit(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordInit(cmd);
	}
}

void CompositCmdRecorder::recordRegular(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordRegular(cmd);
	}
}

void CompositCmdRecorder::recordDynamic(vk::CommandBuffer cmd) {
	for (auto recorder : cmdRecorders) {
		recorder->recordDynamic(cmd);
	}
}

}