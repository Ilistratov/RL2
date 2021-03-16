#include "Utill\UtillFunctions.h"
#include "LogicalBufferResource.h"

namespace Renderer::FrameGraph {

bool BufferAccess::isWrite() const {
	return (access & (
		vk::AccessFlagBits::eHostWrite | 
		vk::AccessFlagBits::eShaderWrite |
		vk::AccessFlagBits::eTransferWrite |
		vk::AccessFlagBits::eColorAttachmentWrite |
		vk::AccessFlagBits::eDepthStencilAttachmentWrite |
		vk::AccessFlagBits::eMemoryWrite
	)) != vk::AccessFlags{};
}

//by default we think, that after the last job comes new iteration and we
//again start perfoming accesses starting from jobId 0 and so on, so
//when we flush/set wait's we need to consider jobs from next/previous iterations
BufferDependency LogicalBufferResource::getWriteDependency(uint32_t jobId) {
	BufferDependency res;
	auto it = jobAccess.find(jobId);

	//scan for preceding reads which we need to wait for
	if (it == jobAccess.begin()) {
		it = std::prev(jobAccess.end());
	} else {
		it = std::prev(it);
	}

	while (!it->second.isWrite()) {
		res.preAccessWait.push_back(it->first);
		it = std::prev(it);
	}

	//scan for folowing reads, for which
	//we need to make our write visible
	it = jobAccess.upper_bound(jobId);

	if (it == jobAccess.end()) {
		it = jobAccess.begin();
	}

	while (!it->second.isWrite()) {
		res.postAccessFlush.dstAccessMask |= it->second.access;
	}

	res.postAccessFlush.srcAccessMask = jobAccess[jobId].access;

	return res;
}

BufferDependency LogicalBufferResource::getReadDependency(uint32_t jobId) {
	//since we flushed all the writes we have to see, we don't need to actually
	//make any dependencies
	return BufferDependency{};
}

void LogicalBufferResource::registerAccess(uint32_t jobId, BufferAccess access) {
	if (jobAccess.count(jobId)) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__,
			"Access for jobId = " +
			std::to_string(jobId) +
			" already registered for this resource")
		);
	}

	usage |= access.usage;
	jobAccess[jobId] = access;
}

BufferDependency LogicalBufferResource::getDependency(uint32_t jobId) {
	if (!jobAccess.count(jobId)) {
		return BufferDependency{};
	}

	if (jobAccess[jobId].isWrite()) {
		return getWriteDependency(jobId);
	}

	return getReadDependency(jobId);
}

}