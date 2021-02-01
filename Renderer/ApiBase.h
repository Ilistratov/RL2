#pragma once

#include <map>

#include <SDL2\SDL.h>
#include <SDL2\SDL_vulkan.h>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer {

class ApiBase {
	vk::UniqueInstance inst;

	vk::PhysicalDevice phDvc;
	vk::UniqueDevice dvc;

	uint32_t grphQInd = UINT32_MAX;
	uint32_t cmptQInd = UINT32_MAX;
	uint32_t prntQInd = UINT32_MAX;

	std::map<uint32_t, vk::Queue> deviceQueues;

	SDL_Window* wnd;
	vk::SurfaceKHR srf;
	vk::UniqueDebugUtilsMessengerEXT dbgUtlMsgr;

	void createInstance(
		std::vector<const char*> instanceExt = {},
		std::vector<const char*> instanceLayer = {}
	);
	
	void createWindow(
		vk::Extent2D extent
	);

	void createSurface();
	
	void createDevice(
		const std::vector<const char*>& deviceExt,
		const std::vector<const char*>& deviceLayers,
		vk::QueueFlags queueFlags
	);
	
	void obtainQueues();

public:
	ApiBase& operator =(ApiBase const&) = delete;
	ApiBase(ApiBase const&) = delete;

	ApiBase();

	vk::Instance instance() const noexcept;

	vk::PhysicalDevice physicalDevice() const noexcept;
	vk::Device device() const noexcept;

	uint32_t queueInd(vk::QueueFlagBits queueBit) const;
	uint32_t presentQInd() const;

	vk::Queue computeQueue() const;
	vk::Queue graphicsQueue() const;
	vk::Queue presentQueue() const;

	SDL_Window* window() const noexcept;
	vk::SurfaceKHR surface() const noexcept;

	uint32_t findMemoryTypeIndex(
		uint32_t typeBits,
		vk::MemoryPropertyFlags propertyBits
	) const;

	~ApiBase();
};

}