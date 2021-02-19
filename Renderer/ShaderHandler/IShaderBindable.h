#pragma once

#include "Renderer\PipelineHandler\DescriptorBinding.h"

namespace Renderer::ShaderHandler {

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
// >|preTransfer barriers|<		|#| Same goes for those stages.
//     |						|#|
//	  \./						|#|
//  |Transfer result|			|#|
//     |						|#|
//	  \./						|#|
//  -repeat-					|#|
//     |						|#|
//	  \./						|#|
// >|preLoad barriers|<			|#|

enum class CBStages {
	Init, //recorded once, submited once
	LoadData, //Recorded once, submited every time
	DynamicLoadData, //Recorded every time for a one time submit
	// Dispatch, is not passed to IShaderBindable as it only contains compute dispatch command and preTransferResult barriers
	TransferResult, // recorded once, submited every time
	DynamicTransferResult //Recorded every time for a one time submit
};

enum class BarrierStages {
	preLoadData,
	preDispatch,
	preTransferResult
};

class IShaderBindable {
public:
	struct Barriers {
		std::vector<vk::BufferMemoryBarrier> buff;
		std::vector<vk::ImageMemoryBarrier> img;
	};

	virtual Barriers collectPreLoadDataBarriers(BarrierStages stage);
	virtual Barriers collectPreDispatchBarriers(BarrierStages stage);
	virtual Barriers collectPreTransferResultBarriers(BarrierStages stage);
	
	virtual void recordInit(vk::CommandBuffer cb);
	
	virtual void recordLoadData(vk::CommandBuffer cb);
	virtual void recordLoadDataDynamic(vk::CommandBuffer cb);

	virtual void recordTransferResult(vk::CommandBuffer cb);
	virtual void recordTransferResultDynamic(vk::CommandBuffer cb);
	
	virtual std::vector<Pipeline::IDescriptorBindable*> collectSetBindables();
	virtual std::vector<vk::PushConstantRange> collectPushConstants();
};

}