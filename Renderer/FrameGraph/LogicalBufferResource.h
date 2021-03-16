#pragma once

#include <map>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::FrameGraph {

struct BufferAccess {
	vk::AccessFlags access;
	vk::BufferUsageFlags usage;
	bool isWrite() const;
};

struct BufferDependency {
	//if we need to write, we have to wait until all preceding reads are finished
	std::vector<uint32_t> preAccessWait;

	//write always makes itself avaliable after is's done and makes
	//writes visible to all folowing reads, until the next write is done
	vk::BufferMemoryBarrier postAccessFlush;
};

class LogicalBufferResource {
	std::map<uint32_t, BufferAccess> jobAccess;
	vk::BufferUsageFlags usage;

	BufferDependency getWriteDependency(uint32_t jobId);
	BufferDependency getReadDependency(uint32_t jobId);

public:
	LogicalBufferResource() = default;

	//jobId represents order
	//in which jobs access resources
	//each job has it's unique jobId
	void registerAccess(uint32_t jobId, BufferAccess access);
	BufferDependency getDependency(uint32_t jobId);
};

}
	