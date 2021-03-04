#pragma once

#include <vector>

#include "ApiBase.h"
#include "Swapchain.h"

#include "Utill\UtillFunctions.h"

namespace Renderer {

//TODO
//Probably should switch from storing ALL the data components
//in the RendererCore, since for some of them
//it makes more sense to store them
//directly where they are used
//like in with cmdPool in ShaderHandler::Compute
class RendererCore {
	ApiBase base;
	SwapchainHndl swpch;

	//std::vector<DataComponent::BufferData> bffs;
	//std::vector<DataComponent::ImageData> imgs;
	//
	//std::vector<DataComponent::CommandPoolData> cmdPools;
	//std::vector<DataComponent::DescriptorPoolData> dPools;
	//
	//std::vector<DataComponent::PipelineData> pplns;

public:
	RendererCore(RendererCore const&) = delete;
	RendererCore operator =(RendererCore const&) = delete;

	RendererCore() = default;
	void init();

	ApiBase& apiBase();
	vk::Device device();

	SwapchainHndl& swapchain();

	//std::vector<DataComponent::BufferData>& getBuffers();
	//std::vector<DataComponent::ImageData>& getImages();
	//std::vector<DataComponent::DescriptorPoolData>& getDescriptorPools();
	//std::vector<DataComponent::CommandPoolData>& getCommandPools();
	//std::vector<DataComponent::PipelineData>& getPipelines();

	~RendererCore();
};

//TODO
//add specializations of destroyDataComponent
//for each used DataComponent

//safety/sanity sacrificed to convenience
//for INTERNAL use ONLY
extern RendererCore core;
const uint64_t TIMEOUT_NSEC = 5'000'000'000ll; // 5 second

}
