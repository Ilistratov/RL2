#pragma once

#include <vector>

#include "DescriptorBinding.h"

namespace Renderer::Pipeline {
/*
* DEPRICATED
* 10-03-2021
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
	
	DPoolHandler(const std::vector<SetBindable>& setBindables);
	
	Data& getData();
	const Data& getData() const;

	~DPoolHandler();

private:
	Data data;
};

*/
}
