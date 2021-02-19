#pragma once

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer {
//TODO
//finish swapchain
//add support for all functionality(well, almost all) that
//the old SwapchainHndl had
class SwapchainHndl {
	vk::SwapchainKHR swpch;

	//since these images are not owned by application, we won't put them in the respective
	//resource storage in Core and will store their handlers directly here
	std::vector<vk::Image> imgs;

	vk::Semaphore imgAvaliable;

	vk::Format fmt = vk::Format::eUndefined;
	vk::Extent2D ext = vk::Extent2D{ 0, 0 };

	uint32_t activeImageInd = UINT32_MAX;

	vk::SwapchainCreateInfoKHR gen_ci() noexcept;
public:
	SwapchainHndl(SwapchainHndl const&) = delete;
	SwapchainHndl& operator =(SwapchainHndl const&) = delete;

	SwapchainHndl() = default;
	void init();

	uint32_t activeImage() const noexcept;
	vk::Semaphore imageAvaliable() const noexcept;
	
	uint32_t imageCount() const noexcept;
	vk::Extent2D extent() const noexcept;
	vk::Format format() const noexcept;

	bool acquireNextImage();
	bool present(std::vector<vk::Semaphore> wait);

	~SwapchainHndl();
};

}