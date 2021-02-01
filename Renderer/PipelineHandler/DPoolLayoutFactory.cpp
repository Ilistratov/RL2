#include <unordered_map>

#include "DPoolLayoutFactory.h"

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
}

vk::DescriptorSetLayoutBinding BindingInfo::getLayoutBinding() const {
	vk::DescriptorSetLayoutBinding res;

	res.binding = bindingId;
	res.descriptorCount = getDescriptorCount();
	res.descriptorType = type;

	return res;
}

DPoolLayoutFactory::DPoolLayoutFactory(uint64_t reservedSize = 0) {
	bindings.reserve(reservedSize);
}

std::vector<vk::DescriptorSetLayoutBinding>&& DPoolLayoutFactory::genLayoutBindings() const {
	std::vector<vk::DescriptorSetLayoutBinding> res;
	res.reserve(bindings.size());
	
	for (const auto& bnd : bindings) {
		res.push_back(bnd.getLayoutBinding());
	}

	return std::move(res);
}

std::vector<vk::WriteDescriptorSet>&& DPoolLayoutFactory::genDescriptorWrites(const std::vector<vk::DescriptorSet>& dSets) const {
	std::vector<vk::WriteDescriptorSet> res;
	res.reserve(bindings.size());
	uint64_t dSetInd = 0;

	for (const auto& bnd : bindings) {
		res.push_back(bnd.getWrite());
		res.back().dstSet = dSets[dSetInd];
		++dSetInd;
	}

	return std::move(res);
}

std::vector<vk::DescriptorPoolSize>&& DPoolLayoutFactory::genPoolSizes() const {
	std::vector<vk::DescriptorPoolSize> res;
	std::unordered_map<vk::DescriptorType, uint64_t> dCnt;

	for (const auto& bnd : bindings) {
		dCnt[bnd.type] += bnd.getDescriptorCount();
	}

	res.reserve(dCnt.size());
	
	for (const auto& [type, cnt] : dCnt) {
		res.push_back(vk::DescriptorPoolSize{ type, cnt });
	}

	return std::move(res);
}

}
