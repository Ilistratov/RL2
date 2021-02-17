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
	CommandPool(uint64_t queueInd);

	CommandPool(CommandPool&& other);
	void operator =(CommandPool&& other);

	void swap(CommandPool& other);
	void free();

	void reserve(uint64_t count);

	Data& getData();

private:
	Data data;
};

}