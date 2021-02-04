#pragma once

#include <vector>

#include "ApiBase.h"
#include "Swapchain.h"

#include "DataComponent\DataComponents.h"

namespace Renderer {

//TODO
//finish swapchain

class RendererCore {
	ApiBase base;
	SwapchainHndl swpch;

	std::vector<DataComponent::BufferData> bffs;
	std::vector<DataComponent::ImageData> imgs;

	std::vector<DataComponent::CommandPoolData> cmdPools;
	std::vector<DataComponent::DescriptorPoolData> dPools;

	std::vector<DataComponent::PipelineData> pplns;

public:
	RendererCore(RendererCore const&) = delete;
	RendererCore operator =(RendererCore const&) = delete;

	RendererCore() = default;
	void init();

	ApiBase& apiBase();
	vk::Device device();

	SwapchainHndl& swapchain();

	std::vector<DataComponent::BufferData>& getBuffers();
	std::vector<DataComponent::ImageData>& getImages();
	std::vector<DataComponent::DescriptorPoolData>& getDescriptorPools();
	std::vector<DataComponent::PipelineData>& getPipelines();

	template<typename T>
	inline void destroyDataComponent(T& component) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__,
			"Can't destroyDataComponent for " + std::string(typeid(T).name()))
		);
	}

	template<typename T>
	inline void destroyDataComponent(std::vector<T>& components) {
		for (auto& component : components) {
			destroyDataComponent(component);
		}
	}

	~RendererCore();
};

//TODO
//add specializations of destroyDataComponent
//for each used DataComponent

template<>
inline void RendererCore::destroyDataComponent(DataComponent::ImageData& component) {
	component.ext = vk::Extent2D{ 0, 0 };
	component.fmt = vk::Format::eUndefined;
	component.sz = 0;
	
	device().destroyImage(component.img);
	device().freeMemory(component.mem);
}

//safety/sanity sacrificed to convenience
//for INTERNAL use ONLY
extern RendererCore core;

}
