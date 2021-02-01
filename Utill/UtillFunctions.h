#pragma once

#include <string>
#include <iostream>
#include <sstream>

//#pragma warning(push, 0)
//#include <vulkan\vulkan.hpp>
//#pragma warning(pop)

std::string gen_err_str(const std::string& file, uint32_t line, const std::string& msg);
//vk::UniqueShaderModule loadShader(vk::Device device, const std::string& filePath);