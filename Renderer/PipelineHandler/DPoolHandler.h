#pragma once

#include <vector>

#include "Renderer\Core.h"
#include "DescriptorBinding.h"

namespace Renderer::Pipeline {

class DPoolHandler {
public:
	struct Data {
		vk::DescriptorPool pool;
		std::vector<vk::DescriptorSet> sets;
		std::vector<vk::DescriptorSetLayout> layts;
	};

	DPoolHandler(const DPoolHandler&) = delete;
	DPoolHandler& operator =(const DPoolHandler&) = delete;
	
	DPoolHandler() = default;

	DPoolHandler(DPoolHandler&& other);
	void operator =(DPoolHandler&& other);

	void swap(DPoolHandler& other);
	void free();
	
	DPoolHandler(const std::vector<IDescriptorBindable*>& setBindables);
	
	Data& getData();
	const Data& getData() const;

	~DPoolHandler();

private:
	Data data;
};

}
