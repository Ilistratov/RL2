#include "Swapchain.h"
#include "Utill\Logger.h"
#include "Utill\UtillFunctions.h"
#include "Core.h"

namespace Renderer {

vk::Format pick_format(std::vector<vk::SurfaceFormatKHR> const& formats) {
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
		return vk::Format::eB8G8R8A8Unorm;
	} else {
		for (const auto& format : formats) {
			if (format.colorSpace != vk::ColorSpaceKHR::eSrgbNonlinear) {
				continue;
			}

			if (format.format == vk::Format::eB8G8R8A8Unorm) {
				return format.format;
			}
		}
	}

	throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Failed to pick surface format"));
	//assert(("Failed to pick surface format", false));
}

vk::Extent2D pick_extent(const vk::SurfaceCapabilitiesKHR& surf_cap) {
	//assert(("surface extent is undefined", surf_cap.currentExtent.width != UINT32_MAX));
	if (surf_cap.currentExtent.width == UINT32_MAX) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "surface extent is undefined"));
	}
	//assert(("surface extent is undefined", surf_cap.currentExtent.width != UINT32_MAX));
	return surf_cap.currentExtent;
}

vk::SwapchainCreateInfoKHR SwapchainHndl::gen_ci() noexcept {
	vk::SwapchainCreateInfoKHR ci{};

	auto surf_cap = core.apiBase().physicalDevice().getSurfaceCapabilitiesKHR(core.apiBase().surface());

	ci.surface = core.apiBase().surface();

	ci.minImageCount = 3;

	ci.imageFormat = fmt = pick_format(core.apiBase().physicalDevice().getSurfaceFormatsKHR(core.apiBase().surface()));
	ci.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

	ci.imageExtent = ext = pick_extent(surf_cap);

	ci.imageArrayLayers = 1;

	ci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
	ci.imageSharingMode = vk::SharingMode::eExclusive;

	ci.preTransform = surf_cap.currentTransform;
	ci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

	if (surf_cap.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
		ci.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	}

	ci.presentMode = vk::PresentModeKHR::eFifo;

	uint32_t q_fam_ind[2] = { core.apiBase().queueInd(vk::QueueFlagBits::eGraphics), core.apiBase().presentQInd() };

	if (q_fam_ind[0] != q_fam_ind[1]) {
		ci.imageSharingMode = vk::SharingMode::eConcurrent;
		ci.queueFamilyIndexCount = 2;
		ci.pQueueFamilyIndices = q_fam_ind;
	}

	return ci;
}

void SwapchainHndl::init() {
	auto device = core.apiBase().device();
	
	swpch = device.createSwapchainKHR(gen_ci());
	imgAvaliable = device.createSemaphore(vk::SemaphoreCreateInfo{});
	imgs = device.getSwapchainImagesKHR(swpch);
}

vk::Extent2D SwapchainHndl::extent() const noexcept {
	return ext;
}

vk::Format SwapchainHndl::format() const noexcept {
	return fmt;
}

vk::ImageMemoryBarrier SwapchainHndl::genInit(uint32_t ind) {
	return vk::ImageMemoryBarrier{
		{},
		{},
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR,
		{VK_QUEUE_FAMILY_IGNORED},
		{VK_QUEUE_FAMILY_IGNORED},
		imgs[ind],
		vk::ImageSubresourceRange{
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		}
	};
}

vk::ImageMemoryBarrier SwapchainHndl::genPreBlit(uint32_t ind) {
	return vk::ImageMemoryBarrier{
		{},
		vk::AccessFlagBits::eTransferWrite,
		vk::ImageLayout::ePresentSrcKHR,
		vk::ImageLayout::eTransferDstOptimal,
		{VK_QUEUE_FAMILY_IGNORED},
		{VK_QUEUE_FAMILY_IGNORED},
		imgs[ind],
		vk::ImageSubresourceRange{
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		}
	};
}

vk::ImageMemoryBarrier SwapchainHndl::genPostBlit(uint32_t ind) {
	return vk::ImageMemoryBarrier{
		vk::AccessFlagBits::eTransferWrite,
		{},
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		{VK_QUEUE_FAMILY_IGNORED},
		{VK_QUEUE_FAMILY_IGNORED},
		imgs[ind],
		vk::ImageSubresourceRange{
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		}
	};
}

ResourceHandler::ImageBase::Data SwapchainHndl::getImageData(uint32_t ind) {
	return ResourceHandler::ImageBase::Data{
		{},
		imgs[ind],
		0,
		ext,
		fmt
	};
}

uint32_t SwapchainHndl::activeImage() const noexcept {
	return activeImageInd;
}

uint32_t SwapchainHndl::imageCount() const noexcept {
	return (uint32_t)imgs.size();
}

vk::Semaphore SwapchainHndl::imageAvaliable() const noexcept {
	return imgAvaliable;
}

bool SwapchainHndl::acquireNextImage() {
	if (activeImageInd != UINT32_MAX) {
		GlobalLog.warningMsg("acquireNextImage called before previously acquired image was presented, keeping old activeImageInd");
		return true;
	}

	auto res = core.apiBase().device().acquireNextImageKHR(swpch, TIMEOUT_NSEC, imgAvaliable, {}, &activeImageInd);

	if (res == vk::Result::eSuccess) {
		return true;
	}

	if (res == vk::Result::eSuboptimalKHR) {
		GlobalLog.debugMsg("eSuboptimalKHR while acquireNextImage(), swapchain needs to be updated");
		activeImageInd = UINT32_MAX;
		return false;
	}

	GlobalLog.errorMsg(std::string("Error during acquireNextImage(), vkResult code: ") + std::to_string(((uint32_t)(res))));
	throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Error during acquireNextImage()" + std::to_string((uint32_t)res)));
	//assert(("Error during acquireNextImage()" + (uint32_t)res, false));
}

bool SwapchainHndl::present(std::vector<vk::Semaphore> wait) {
	vk::Result res;
	try {
		res = core.apiBase().presentQueue().presentKHR(vk::PresentInfoKHR{
			(uint32_t)wait.size(),
			wait.data(),
			1,
			&swpch,
			&activeImageInd,
			{}
			}
		);
	} catch (const std::exception&) {
		return false;
	}

	if (res == vk::Result::eSuccess) {
		activeImageInd = UINT32_MAX;
		return true;
	} else if (res == vk::Result::eSuboptimalKHR) {
		return false;
	}

	GlobalLog.errorMsg(std::string("Error during present(), vkResult code: " + std::to_string((uint32_t)res)));
	throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Error during present(), vkResult code: " + std::to_string((uint32_t)res)));
	//assert(("Error during present()", false));
}

SwapchainHndl::~SwapchainHndl() {
	auto device = core.apiBase().device();
	device.destroySemaphore(imgAvaliable);
	device.destroySwapchainKHR(swpch);
}

}