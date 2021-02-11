#pragma once

#include "Renderer\Core.h"
#include "Renderer\PipelineHandler\DSetlLayoutFactory.h"
#include "Renderer\ResourceHandler\CommandPool.h"

namespace Renderer::ShaderHandler {

struct PipelineBarrierBatch {
	std::vector<vk::BufferMemoryBarrier> buff;
	std::vector<vk::ImageMemoryBarrier> img;
};

struct ComputeFactory {
	std::vector<Pipeline::DSetLayoutFactory> layoutFactory;

	PipelineBarrierBatch preTransferBrrBatch;
	PipelineBarrierBatch preDispatchBrrBatch;

	std::vector<IVarControllerCallback*> vcCallback;

	ComputeFactory(uint64_t dSetCount);
};

class IVarControllerCallback {
public:
	virtual void onVariableRegistration(ComputeFactory& cf) = 0;
	virtual void onTransferRecord(vk::CommandBuffer cbTransfer) = 0;
	virtual void onInitRecord(vk::CommandBuffer cbInit) = 0;
	//virtual bool onDispatchRecord(vk::CommandBuffer transferCb);
};

}