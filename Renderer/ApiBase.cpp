#include <set>

#include "ApiBase.h"

#include "Utill\Logger.h"
#include "Utill\UtillFunctions.h"

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger) {
	return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT      messenger,
	VkAllocationCallbacks const* pAllocator) {
	return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

namespace Renderer {

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
	void* /*pUserData*/) {
	//if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		GlobalLog.errorMsg(pCallbackData->pMessage);
	} else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		GlobalLog.warningMsg(pCallbackData->pMessage);
	} else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		GlobalLog.infoMsg(pCallbackData->pMessage);
	} else {
		GlobalLog.debugMsg(pCallbackData->pMessage);
	}
	//}

	return VK_FALSE;
}

void ApiBase::createInstance(
	std::vector<const char*> instanceExt,
	std::vector<const char*> instanceLayer) {
	vk::ApplicationInfo application_i{};
	application_i.setApiVersion(VK_API_VERSION_1_2);
	application_i.setPApplicationName("RL2");
	application_i.setPEngineName("RL2_RayTrace_Renderer");

	vk::InstanceCreateInfo instance_ci{};
	instance_ci.setPApplicationInfo(&application_i);

#ifndef NDEBUG
	{
		instanceExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		instanceExt.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		instanceLayer.push_back("VK_LAYER_KHRONOS_validation");
	}
#endif // !NDEBUG

	{
		uint32_t ext_cnt = 0;
		SDL_Vulkan_GetInstanceExtensions(wnd, &ext_cnt, nullptr);
		std::vector<const char*> ext_names(ext_cnt);
		SDL_Vulkan_GetInstanceExtensions(wnd, &ext_cnt, ext_names.data());

		instanceExt.insert(instanceExt.end(), ext_names.begin(), ext_names.end());
	}

	instance_ci.enabledExtensionCount = (uint32_t)instanceExt.size();
	instance_ci.ppEnabledExtensionNames = instanceExt.data();

	instance_ci.enabledLayerCount = (uint32_t)instanceLayer.size();
	instance_ci.ppEnabledLayerNames = instanceLayer.data();

#ifndef NDEBUG
	{
		std::stringstream ss;
		ss << "Creating instance\nWith folowing instance extensions:\n";
		Logger::addVector(ss, instanceExt);
		ss << "Folowing instance layers\n";
		Logger::addVector(ss, instanceLayer);
		GlobalLog.debugMsg(ss.str());
	}
#endif // !NDEBUG

	try {
		inst = vk::createInstanceUnique(instance_ci);
	} catch (const std::exception& e) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, e.what()));
	} catch (...) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "unknown error"));
	}

#ifndef NDEBUG
	{
		pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
			inst.get().getProcAddr("vkCreateDebugUtilsMessengerEXT")
			);
		pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			inst.get().getProcAddr("vkDestroyDebugUtilsMessengerEXT")
			);

		assert(("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function", pfnVkCreateDebugUtilsMessengerEXT));
		assert(("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function", pfnVkDestroyDebugUtilsMessengerEXT));

		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
			//vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

		dbgUtlMsgr = inst.get().createDebugUtilsMessengerEXTUnique(
			vk::DebugUtilsMessengerCreateInfoEXT({},
				severityFlags,
				messageTypeFlags,
				&debugMessageFunc));
	}
#endif // !NDEBUG

	GlobalLog.infoMsg("Created Instance");
}

void ApiBase::createWindow(
	vk::Extent2D extent) {


	wnd = SDL_CreateWindow(
		"RL2_WND_Name_Configure_TBD",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		extent.width, extent.height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
	);

	if (!wnd) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Failed to create window"));
	}

	GlobalLog.infoMsg("Created window");
}

void ApiBase::createSurface() {
	VkSurfaceKHR srf_raw = VK_NULL_HANDLE;

	if (!SDL_Vulkan_CreateSurface(wnd, inst.get(), &srf_raw)) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Failed to create window surface"));
	}

	srf = vk::SurfaceKHR(srf_raw);

	GlobalLog.infoMsg("Created surface");
}


bool checkDeviceExtensionsPresented(vk::PhysicalDevice ph_dvc, std::vector<const char*> deviceExt) {
	std::set<std::string> requiered_ext;
	for (auto ext : deviceExt) {
		requiered_ext.insert(std::string(ext));
	}

	auto available_ext = ph_dvc.enumerateDeviceExtensionProperties();

	for (const auto& ext : available_ext) {
		requiered_ext.erase(std::string(ext.extensionName.data()));
	}

	if (!requiered_ext.empty()) {
		std::stringstream msg;
		msg << "Requested Extensions not presented:\n";
		std::vector<std::string> ext(requiered_ext.begin(), requiered_ext.end());
		Logger::addVector(msg, ext);
		GlobalLog.debugMsg(msg.str());
	}

	return requiered_ext.empty();
}

bool checkDeviceRequieredPropertiesProvided(vk::PhysicalDevice device, vk::SurfaceKHR surface, vk::QueueFlags queueFlags) {
	auto device_features = device.getFeatures();

	if (!device_features.geometryShader) {
		GlobalLog.debugMsg("No geometry shader feature");
		return false;
	}

	//if (!device_features.samplerAnisotropy) {
	//	return false;
	//}

	//TODO
	if (device.getSurfacePresentModesKHR(surface).empty() || device.getSurfaceFormatsKHR(surface).empty()) {
		GlobalLog.debugMsg("No surface present modes/formats found");
		return false;
	}

	auto queue_properties = device.getQueueFamilyProperties();

	vk::QueueFlags presented_queue_flags;

	for (const auto& device_queue : queue_properties) {
		presented_queue_flags |= device_queue.queueFlags;
	}

	if ((queueFlags & vk::QueueFlagBits::eCompute) && !(presented_queue_flags & vk::QueueFlagBits::eCompute)) {
		GlobalLog.debugMsg("Compute queue requested but not found");
		return false;
	}

	if ((queueFlags & vk::QueueFlagBits::eGraphics) && !(presented_queue_flags & vk::QueueFlagBits::eGraphics)) {
		GlobalLog.debugMsg("Graphics queue requested but not found");
		return false;
	}

	if ((queueFlags & vk::QueueFlagBits::eTransfer) && !(presented_queue_flags & vk::QueueFlagBits::eTransfer)) {
		GlobalLog.debugMsg("Transfer queue requested but not found");
		return false;
	}

	return true;
}

uint64_t calcDeviceMemSize(vk::PhysicalDevice device) {
	uint64_t res = 0;
	auto mem_prop = device.getMemoryProperties();

	for (const auto& heap : mem_prop.memoryHeaps) {
		res += heap.size;
	}

	return res;
}

void ApiBase::createDevice(
	const std::vector<const char*>& deviceExt,
	const std::vector<const char*>& deviceLayers,
	vk::QueueFlags queueFlags) {
	GlobalLog.debugMsg("Searching for physical device");

	auto devices = inst.get().enumeratePhysicalDevices();
	uint64_t device_mem_size = 0;
	vk::PhysicalDeviceProperties device_prop;

	for (const auto& device : devices) {
		auto properties = device.getProperties();
		GlobalLog.debugMsg("Checking " + std::string(properties.deviceName.data()));

		if (!checkDeviceExtensionsPresented(device, deviceExt)) {
			continue;
		}

		if (!checkDeviceRequieredPropertiesProvided(device, srf, queueFlags)) {
			continue;
		}

		if ((VkPhysicalDevice)phDvc == VK_NULL_HANDLE) {
			phDvc = device;
			device_mem_size = calcDeviceMemSize(device);
			device_prop = properties;
		} else if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			uint64_t cur_dvc_mem_size = calcDeviceMemSize(device);
			if (!(device_prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) || cur_dvc_mem_size > device_mem_size) {
				phDvc = device;
				device_mem_size = cur_dvc_mem_size;
			}
		}
	}

	assert(("Failed to find siutable physical device", (VkPhysicalDevice)phDvc != VK_NULL_HANDLE));

	GlobalLog.debugMsg("Using " + std::string(device_prop.deviceName.data()) + " as physical device");

	std::vector<vk::DeviceQueueCreateInfo> queue_ci;

	auto device_queues = phDvc.getQueueFamilyProperties();
	bool hasPresent = false;
	float queue_priority = 1.0;

	for (uint32_t q_ind = 0; q_ind < device_queues.size(); q_ind++) {
		bool presentSupport = phDvc.getSurfaceSupportKHR(q_ind, srf);

		if (!hasPresent && presentSupport) {
			if (queue_ci.empty() || queue_ci.back().queueFamilyIndex != q_ind) {
				vk::DeviceQueueCreateInfo q_ci{};
				q_ci.queueCount = 1;
				q_ci.queueFamilyIndex = q_ind;
				q_ci.queueCount = 1;
				q_ci.pQueuePriorities = &queue_priority;

				prntQInd = q_ind;
				hasPresent = true;

				queue_ci.push_back(q_ci);
			}
		}

		auto update_queue_ci_lambda = [&device_queues, &q_ind, &queueFlags, &queue_ci](vk::QueueFlagBits queueBit) {
			if ((queueFlags & queueBit) && (device_queues[q_ind].queueFlags & queueBit)) {
				queueFlags ^= queueBit;
				if (queue_ci.empty() || queue_ci.back().queueFamilyIndex != q_ind) {
					vk::DeviceQueueCreateInfo q_ci{};
					q_ci.queueCount = 1;
					q_ci.queueFamilyIndex = q_ind;

					queue_ci.push_back(q_ci);
				}

				return true;
			}

			return false;
		};

		if (update_queue_ci_lambda(vk::QueueFlagBits::eGraphics)) {
			grphQInd = q_ind;
		}

		if (update_queue_ci_lambda(vk::QueueFlagBits::eCompute)) {
			cmptQInd = q_ind;
		}
	}

	vk::PhysicalDeviceFeatures ph_dvc_ftr;
	ph_dvc_ftr.shaderStorageImageWriteWithoutFormat = true;

	vk::DeviceCreateInfo ci{};
	ci.enabledExtensionCount = (uint32_t)deviceExt.size();
	ci.ppEnabledExtensionNames = deviceExt.data();

	ci.enabledLayerCount = (uint32_t)deviceLayers.size();
	ci.ppEnabledLayerNames = deviceLayers.data();

	ci.queueCreateInfoCount = (uint32_t)queue_ci.size();
	ci.pQueueCreateInfos = queue_ci.data();

	ci.pEnabledFeatures = &ph_dvc_ftr;

	dvc = phDvc.createDeviceUnique(ci);
}

void ApiBase::obtainQueues() {
	GlobalLog.debugMsg("obtaining deviceQueue");

	if (grphQInd != UINT32_MAX) {
		deviceQueues[grphQInd] = device().getQueue(grphQInd, 0);
		GlobalLog.debugMsg("obtained graphics queue");
	}

	if (cmptQInd != UINT32_MAX) {
		deviceQueues[cmptQInd] = device().getQueue(cmptQInd, 0);
		GlobalLog.debugMsg("obtained compute queue");
	}

	if (prntQInd != UINT32_MAX) {
		deviceQueues[prntQInd] = device().getQueue(prntQInd, 0);
		GlobalLog.debugMsg("obtained present queue");
	}
}

ApiBase::ApiBase() {
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Failed to SDL_Init()"));
	}

	createWindow({ 1280, 768 });
	createInstance({}, {});
	createSurface();
	createDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, {}, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);
	obtainQueues();
}

vk::Instance ApiBase::instance() const noexcept {
	return inst.get();
}

vk::PhysicalDevice ApiBase::physicalDevice() const noexcept {
	return phDvc;
}

vk::Device ApiBase::device() const noexcept {
	return dvc.get();
}

uint32_t ApiBase::queueInd(vk::QueueFlagBits queueBit) const {
	if (queueBit == vk::QueueFlagBits::eCompute) {
		return cmptQInd;
	} else if (queueBit == vk::QueueFlagBits::eGraphics) {
		return grphQInd;
	} else {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "Unknown queue type"));
	}
}

uint32_t ApiBase::presentQInd() const {
	return prntQInd;
}

vk::Queue ApiBase::computeQueue() const {
	return deviceQueues.at(cmptQInd);
}

vk::Queue ApiBase::graphicsQueue() const {
	return deviceQueues.at(grphQInd);
}

vk::Queue ApiBase::presentQueue() const {
	return deviceQueues.at(prntQInd);
}

SDL_Window* ApiBase::window() const noexcept {
	return wnd;
}

vk::SurfaceKHR ApiBase::surface() const noexcept {
	return srf;
}

uint32_t ApiBase::findMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags propertyBits) const {
	auto memProperties = phDvc.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeBits & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & propertyBits) == propertyBits) {
			return i;
		}
	}

	throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "failed to find suitable memory type"));
}

//TODO need some sort of callback passed here
//so that Renderer Registry can be
//signaled to free all the resources it stores
//since in vulkan vk::Device is needed
//quite often to free resources
//and we don't want to ship a reference to
//it with every single object we create
//since it's basicly a waste of memory
ApiBase::~ApiBase() {
	dvc.get().waitIdle();
	inst.get().destroySurfaceKHR(srf);
	SDL_DestroyWindow(wnd);
	SDL_Quit();
}

}