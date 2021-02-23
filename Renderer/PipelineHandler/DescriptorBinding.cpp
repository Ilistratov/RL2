#include "DescriptorBinding.h"
#include <cassert>

namespace Renderer::Pipeline {

DescriptorBinding::DescriptorBinding(
	std::vector<vk::DescriptorBufferInfo> buff,
	std::vector<vk::DescriptorImageInfo> img,
	vk::ShaderStageFlags stage,
	vk::DescriptorType type) :
	buff(buff),
	img(img),
	stage(stage),
	type(type) {
	assert((img.empty() != buff.empty(), "One and only one of img/buff must contain a minimum of one element"));
}

void DescriptorBinding::setBindingId(uint32_t nId) {
	bindingId = nId;
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

//std::vector<DescriptorBinding> IDescriptorBindable::getBindings() const { return {}; }

SetBindable::SetBindable(const std::vector<IDescriptorBindable*>& bnd) {
	bindings.resize(bnd.size());
	
	for (uint32_t i = 0; i < bnd.size(); i++) {
		bindings[i] = bnd[i]->getBinding();
		bindings[i].setBindingId(i);
	}
}

const std::vector<DescriptorBinding>& SetBindable::getBindings() const {
	return bindings;
}

}