#pragma once

#include "Renderer\PipelineHandler\DescriptorBinding.h"

namespace Renderer::ShaderBindable {

// |Init| at start					  
//
// shaderInput dynamic				  shaderOutput dynamic				 shaderInput dynamic
// shaderInput regular -> dispatch -> shaderOutput regular				 shaderInput regular
// shaderOutput regular				  shaderInput dynamic -> dispatch -> shaderOutput dynamic 
// shaderOutput dynamic				  shaderInput regular				 shaderOutput regular

//Represents object
//that can be used as shader input
//regular cmd recorded once
//and submited each time before dispatch
//dynamic recoreded and submited
//each time before dispatch
class IShaderInput {
public:
	virtual void recordInit(vk::CommandBuffer cmd);

	//cmd executed on transfer stage before dispatch
	//if needed, preLoad barriers as well as preDispatch barriers
	//should be also recorded here
	virtual void recordRegular(vk::CommandBuffer cmd);
	virtual void recordDynamic(vk::CommandBuffer cmd);
};

class IShaderOutput {
public:
	virtual void recordInit(vk::CommandBuffer cmd);

	//cmd executed on transfer stage before dispatch
	//if needed, preLoad barriers as well as preDispatch barriers
	//should be also recorded here
	virtual void recordRegular(vk::CommandBuffer cmd);
	virtual void recordDynamic(vk::CommandBuffer cmd);
};

// Even more generic way is possible
// An abstract class with similar interface as IShaderInput(Output)
// which describes an arbitrary stage, 
// array of these is sent to an executor along with dependency graph
// executor then submits array consequently
// adding wait on timelined semaphores
// for specified in dependency graph stages
// and signals timelined semaphore with incremented value
// while it seems as a good idea to even further generalize
// the way workload is submited to gpu, right now
// it is not necessary since
// i have no idea what stages other than two presented here
// and a fixed dispatch stage can possibly be used.
}