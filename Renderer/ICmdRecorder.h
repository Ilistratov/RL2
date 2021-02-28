#pragma once

#include <vector>

#include "Core.h"

namespace Renderer {

//Represents object
//that can be used as shader input
//regular cmd recorded once
//and submited each time before dispatch
//dynamic recoreded and submited
//each time before dispatch
class ICmdRecorder {
public:
	//should be independent from recordInit of other 
	//ICmdRecorder's as no ordering / sync is guaranteed for
	//this call
	virtual void recordInit(vk::CommandBuffer cmd);

	virtual void recordRegular(vk::CommandBuffer cmd);
	virtual void recordDynamic(vk::CommandBuffer cmd);
};

class CompositCmdRecorder : public ICmdRecorder {
	std::vector<ICmdRecorder*> cmdRecorders;
public:
	CompositCmdRecorder() = default;
	CompositCmdRecorder(const std::vector<ICmdRecorder*>& cmdRecorders);

	void recordInit(vk::CommandBuffer cmd) override;

	void recordRegular(vk::CommandBuffer cmd) override;
	void recordDynamic(vk::CommandBuffer cmd) override;
};

}