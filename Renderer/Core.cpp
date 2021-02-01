#include "Core.h"

namespace Renderer {

void RendererCore::init() {
	swpch.init();
}

ApiBase& RendererCore::apiBase() {
	return base;
}

vk::Device RendererCore::device() {
	return base.device();
}

SwapchainHndl& RendererCore::swapchain() {
	return swpch;
}

std::vector<DataComponent::BufferData>& RendererCore::getBuffers() {
	return bffs;
}

RendererCore::~RendererCore() {
	device().waitIdle();

	destroyDataComponent(cmdPools);
	destroyDataComponent(dPools);
	destroyDataComponent(imgs);
	destroyDataComponent(bffs);
}

RendererCore core;

}