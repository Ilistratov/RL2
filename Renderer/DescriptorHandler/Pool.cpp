#include "Utill\UtillFunctions.h"

#include "Renderer\Core.h"
#include "Pool.h"

namespace Renderer::DescriptorHandler {

Pool::Pool(const Layout& layt) {
	pool = core.device().createDescriptorPool(
		vk::DescriptorPoolCreateInfo{
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			layt.getSetCount(),
			layt.getSizes()
		}
	);

	sets = core.device().allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo{
			pool,
			layt.getLayouts()
		}
	);
}

void Pool::swap(Pool& other) {
	std::swap(pool, other.pool);
	sets.swap(other.sets);
}

void Pool::free() {
	if (!sets.empty()) {
		core.device().freeDescriptorSets(pool, sets);
	}

	sets.clear();
	sets.shrink_to_fit();

	core.device().destroyDescriptorPool(pool);
	pool = vk::DescriptorPool{};
}

Pool::Pool(Pool&& other) {
	swap(other);
}

void Pool::operator=(Pool&& other) {
	if (this == &other) {
		return;
	}

	swap(other);
	other.free();
}

Pool::~Pool() {
	free();
}

vk::WriteDescriptorSet makeDescriptorWriteVk(vk::DescriptorSet set, uint32_t bindingId, const DescriptorWrite& write) {
	if (!write.imageInfo.empty()) {
		return vk::WriteDescriptorSet{
			set,
			bindingId,
			write.dstArrayElement,
			write.descriptorCount,
			write.type,
			write.imageInfo.data()
		};
	} else if (!write.bufferInfo.empty()) {
		return vk::WriteDescriptorSet{
			set,
			bindingId,
			write.dstArrayElement,
			write.descriptorCount,
			write.type,
			nullptr,
			write.bufferInfo.data()
		};
	} else {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, "At least one of imageInfo, bufferInfo must not be empty"));
	}
}

void Pool::write(uint32_t setId, uint32_t bindingId, const DescriptorWrite& dbWrite) {
	core.device().updateDescriptorSets(makeDescriptorWriteVk(sets[setId], bindingId, dbWrite), {});
}

void Pool::write(uint32_t setId, const std::vector<DescriptorWrite>& dWrites) {
	std::vector<vk::WriteDescriptorSet> writesVk(dWrites.size());
	for (uint32_t i = 0; i < dWrites.size(); i++) {
		writesVk[i] = makeDescriptorWriteVk(sets[setId], i, dWrites[i]);
	}

	core.device().updateDescriptorSets(writesVk, {});
}

void Pool::write(const std::vector<std::vector<DescriptorWrite>>& sWrites) {
	for (uint32_t i = 0; i < sWrites.size(); i++) {
		write(i, sWrites[i]);
	}
}

const std::vector<vk::DescriptorSet>& Pool::getSets() const {
	return sets;
}

}