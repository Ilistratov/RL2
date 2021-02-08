#include <unordered_map>

#include "DSetlLayoutFactory.h"

namespace Renderer::Pipeline {

uint64_t BindingInfo::getDescriptorCount() const {
	assert((imageInfo.empty() != bufferInfo.empty(), "One and only one of imageInfo, bufferInfo, must contain a minimum of one element"));
	return imageInfo.size() + bufferInfo.size();
}

vk::WriteDescriptorSet BindingInfo::getWrite() const {
	vk::WriteDescriptorSet res;

	res.dstBinding = bindingId;
	res.descriptorCount = getDescriptorCount();
	res.descriptorType = type;
	res.dstArrayElement = 0;
	res.pBufferInfo = bufferInfo.data();
	res.pImageInfo = imageInfo.data();

	return res;
}

vk::DescriptorSetLayoutBinding BindingInfo::getLayoutBinding() const {
	vk::DescriptorSetLayoutBinding res;

	res.binding = bindingId;
	res.descriptorCount = getDescriptorCount();
	res.descriptorType = type;

	return res;
}

DSetLayoutFactory::DSetLayoutFactory(uint64_t reservedSize) {
	bindings.reserve(reservedSize);
}

std::vector<vk::DescriptorSetLayoutBinding>&& DSetLayoutFactory::genLayoutBindings() const {
	std::vector<vk::DescriptorSetLayoutBinding> res;
	res.reserve(bindings.size());
	
	for (const auto& bnd : bindings) {
		res.push_back(bnd.getLayoutBinding());
	}

	return std::move(res);
}

std::vector<vk::WriteDescriptorSet>&& DSetLayoutFactory::genDescriptorWrites(vk::DescriptorSet dSet) const {
	std::vector<vk::WriteDescriptorSet> res;
	res.reserve(bindings.size());

	for (const auto& bnd : bindings) {
		res.push_back(bnd.getWrite());
		res.back().dstSet = dSet;
	}

	return std::move(res);
}

std::unordered_map<vk::DescriptorType, uint64_t>&& DSetLayoutFactory::genPoolSizes() const {
	std::unordered_map<vk::DescriptorType, uint64_t> dCnt;

	for (const auto& bnd : bindings) {
		dCnt[bnd.type] += bnd.getDescriptorCount();
	}

	return std::move(dCnt);
}

}
