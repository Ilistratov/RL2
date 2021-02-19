#include "DescriptorBinding.h"
#include <cassert>

namespace Renderer::Pipeline {

DescriptorBinding::DescriptorBinding(
	std::vector<vk::DescriptorBufferInfo> buff,
	std::vector<vk::DescriptorImageInfo> img,
	uint32_t bindingId,
	vk::ShaderStageFlags stage,
	vk::DescriptorType type) :
	buff(buff),
	img(img),
	bindingId(bindingId),
	stage(stage),
	type(type) {
	assert((img.empty() != buff.empty(), "One and only one of img/buff must contain a minimum of one element"));
}


uint32_t DescriptorBinding::getCount() const {
	return img.size() + buff.size();
}

vk::WriteDescriptorSet DescriptorBinding::getWrite() const {
	return vk::WriteDescriptorSet{
		vk::DescriptorSet{}, //dSet will be assigned by dPoolHandler when created
		bindingId,
		0, //as this is used only when dSet's are created. In order to write to existing dSet, another class will be used
		type,
		img,
		buff
	};
}

vk::DescriptorSetLayoutBinding DescriptorBinding::getLayoutBinding() const {
	return vk::DescriptorSetLayoutBinding{
		bindingId,
		type,
		stage,
		{} //unused as yet to be understood
	};
}

std::vector<DescriptorBinding> IDescriptorBindable::getBindings() const { return {}; }

}