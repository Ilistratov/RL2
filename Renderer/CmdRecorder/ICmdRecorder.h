#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::CmdRecorder {

class ICmdRecorder {
public:
	virtual void record(vk::CommandBuffer cmd);
};

class CompositCmdRecorder : public ICmdRecorder {
	std::vector<ICmdRecorder*> cmdRecorders;
public:
	CompositCmdRecorder() = default;
	CompositCmdRecorder(const std::vector<ICmdRecorder*>& cmdRecorders);

	void record(vk::CommandBuffer cmd) override;
};

}