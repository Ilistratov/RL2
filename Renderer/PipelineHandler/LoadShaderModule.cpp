#include <fstream>

#include "Renderer\Core.h"
#include "LoadShaderModule.h"
#include "Utill\UtillFunctions.h"

namespace Renderer::Pipeline {

vk::UniqueShaderModule loadShader(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.good()) {
		throw std::runtime_error(gen_err_str(__FILE__, __LINE__, std::string("Failed to open ") + filePath));
	}

	//assert((std::string("Failed to open ") + filePath, file.is_open()));

	size_t fileSize = file.tellg();
	std::vector<char> shaderCode(fileSize);

	file.seekg(0);
	file.read(shaderCode.data(), fileSize);
	file.close();

	return std::move(
		core.device().createShaderModuleUnique(
			vk::ShaderModuleCreateInfo{
				vk::ShaderModuleCreateFlags{},
				shaderCode.size(),
				(const uint32_t*)shaderCode.data()
			}
		)
	);
}

}