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
RendererCore::~RendererCore() {
	device().waitIdle();
}

RendererCore core;

}