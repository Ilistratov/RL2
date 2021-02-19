#pragma once
#include "Renderer\Core.h"

namespace Renderer::ResourceHandler {

class CommandPool {
public:
	struct Data {
		vk::CommandPool pool;
		std::vector<vk::CommandBuffer> cmd;
	};

	CommandPool(const CommandPool&) = delete;
	CommandPool& operator =(const CommandPool&) = delete;

	CommandPool() = default;
	CommandPool(uint32_t queueInd);

	CommandPool(CommandPool&& other);
	void operator =(CommandPool&& other);

	void swap(CommandPool& other);
	void free();

	void reserve(uint32_t count);
	vk::CommandBuffer reserveOneTimeSubmit();
	void freeOneTimeSubmit(std::vector<vk::CommandBuffer> cmd);

	Data& getData();

	~CommandPool();
private:
	Data data;
};

}