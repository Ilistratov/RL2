#include "Renderer\Core.h"
#include "PushConstant.h"

namespace Renderer::ShaderBindable {

PushConstantController::PushConstantController(std::vector<IPushConstant*> pushConstants) : pushConstants(pushConstants) {
	uint64_t curOffset = 0;

	for (auto pc : pushConstants) {
		pc->setOffset(curOffset);
		curOffset += pc->getSize();
	}
}

void PushConstantController::recordDynamic(vk::CommandBuffer cmd) {
	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, boundPpln);
	for (auto pc : pushConstants) {
		pc->recordPush(cmd, boundLayt);
	}
}

void PushConstantController::recordInit(vk::CommandBuffer cmd) {
	recordDynamic(cmd);
}

void PushConstantController::bindLayout(vk::PipelineLayout layt) {
	boundLayt = layt;
}

void PushConstantController::bindPipeline(vk::Pipeline ppln) {
	boundPpln = ppln;
}

std::vector<vk::PushConstantRange> PushConstantController::getPCR() {
	std::vector<vk::PushConstantRange> res;
	res.reserve(pushConstants.size());

	for (auto pc : pushConstants) {
		res.push_back(pc->getPCR());
	}

	return res;
}

}