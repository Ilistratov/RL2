cmake_minimum_required (VERSION 3.8)

set(SRCS 
	IShaderControlable.h
	IShaderControlable.cpp
	StructuredBuffer.cpp
	StructuredBuffer.h
	RenderTarget.cpp
	RenderTarget.h
)

add_library(ShaderBindable ${SRCS})

target_include_directories(ShaderBindable PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(ShaderBindable  PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET ShaderBindable  PROPERTY CXX_STANDARD 17)
