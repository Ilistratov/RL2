#pragma once

#include <vector>

#pragma warning(push, 0)
#include <vulkan\vulkan.hpp>
#pragma warning(pop)

namespace Renderer::DescriptorHandler {

struct LayoutBinding {
	vk::DescriptorType type;
	uint32_t count;
	vk::ShaderStageFlags stage;
};

class Layout {
	std::vector<vk::DescriptorSetLayout> layt;
	std::vector<vk::DescriptorPoolSize> size;

public:
	Layout() = default;
	Layout(const std::vector<std::vector<LayoutBinding>>& bindings);

	Layout(const Layout&) = delete;
	Layout& operator =(const Layout&) = delete;

	void swap(Layout& other);
	void free();

	Layout(Layout&& other);
	void operator =(Layout&& other);

	~Layout();

	const std::vector<vk::DescriptorSetLayout>& getLayouts() const;
	const std::vector<vk::DescriptorPoolSize>& getSizes() const;
	uint32_t getSetCount() const;
};

}