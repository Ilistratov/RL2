cmake_minimum_required (VERSION 3.8)

set(SRCS 
	Compute.h
	Compute.cpp
	IShaderBindable.h
	IShaderBindable.cpp
)

add_library(ShaderHandler ${SRCS})

target_include_directories(ShaderHandler PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(ShaderHandler  PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET ShaderHandler  PROPERTY CXX_STANDARD 17)
