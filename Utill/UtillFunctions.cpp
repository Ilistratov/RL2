#include <fstream>

#include "UtillFunctions.h"

std::string gen_err_str(const std::string& file, uint32_t line, const std::string& msg) {
	std::string err = msg;
	if (err.back() != '\n') {
		err += '\n';
	}
	err += "At";
	err += file;
	err += " Line: ";
	err += std::to_string(line);
	err += "\n";
	return err;
}

//vk::UniqueShaderModule loadShader(vk::Device device, const std::string& filePath) {
//	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
//
//	assert((std::string("Failed to open ") + filePath, file.is_open()));
//
//	size_t fileSize = file.tellg();
//	std::vector<char> shaderCode(fileSize);
//
//	file.seekg(0);
//	file.read(shaderCode.data(), fileSize);
//	file.close();
//
//	return std::move(device.createShaderModuleUnique(
//		vk::ShaderModuleCreateInfo{
//			vk::ShaderModuleCreateFlags{},
//			shaderCode.size(),
//			(const uint32_t*)shaderCode.data()
//		}
//	));
//}
