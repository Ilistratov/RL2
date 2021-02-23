#pragma once

#include "Renderer\PipelineHandler\DescriptorBinding.h"

namespace Renderer::ShaderBindable {

// |Init| at start
//
//  -repeat-
//     |
//	  \./
// >|preLoad barriers|<			|#| Sometimes commands on those stages
//     |						|#| can't be recorded only once and
//	  \./						|#| instead should be recorded and submited
//  |Load data|					|#| when needed. IShaderBindable if needed
//     |						|#| can record such buffers. It might be
//	  \./						|#| necessary to also record there some commands, that
// >|preDispatch barriers|<		|#| usually appear in other two of the marked stages
//     |
//	  \./
//  |Dispatch|
//     |
//	  \./
// >|preTransfer barriers|<		|#| Same goes for this stages.
//     |						|#|
//	  \./						|#|
//  |Transfer result|			|#|
//     |						|#|
//	  \./						|#|
//  -repeat-					|#|
//     |						|#|
//	  \./						|#|
// >|preLoad barriers|<			|#|

class IShaderControlable {
public:
	struct Barriers {
		std::vector<vk::BufferMemoryBarrier> buff;
		std::vector<vk::ImageMemoryBarrier> img;

		void record(vk::CommandBuffer& cmd, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage);
	};

	//as it always possible to avoid usage of one variable
	//both as an input and as an output of a shader(just make two, on for each purpose)
	//which means that
	//PreLoadDataBarriers are rather preventing race condition
	//between Compute and LoadData, than between TransferResult and LoadData
	//so PipelineStageFlagBits::eCompute is used as srcStage
	virtual Barriers&& collectPreLoadDataBarriers();
	virtual Barriers&& collectPreDispatchBarriers();
	virtual Barriers&& collectPreTransferResultBarriers();
	
	virtual void recordInit(vk::CommandBuffer cmd);						// |#| This functions wont be called simultaneously from multiple threads
																		// |#| (unless you pass it to multiple ShaderHandler's)
	virtual void recordLoadData(vk::CommandBuffer cmd);					// |#| but multiple different functions can be called from different threads
	virtual void recordLoadDataDynamic(vk::CommandBuffer cmd);			// |#| ex. recordLoadData and recordTransferResult are called at the same
																		// |#| time from two different threads. However, this doesn't apply to recordInit
	virtual void recordTransferResult(vk::CommandBuffer cmd);			// |#| which will be called once, prior to all other record calls
	virtual void recordTransferResultDynamic(vk::CommandBuffer cmd);	// |#| 
};

//TODO
//make separate classes for controling shader variables and for binding them initially
//binding can be done via this class
//controlling via IShaderControlable

//class IControlableDescriptorBindable : public IShaderControlable, public Pipeline::IDescriptorBindable {};
//class IControlableShaderBindable : public IShaderControlable, public IShaderBindable {};

}