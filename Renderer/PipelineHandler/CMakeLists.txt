cmake_minimum_required (VERSION 3.8)

set(SRCS 
	Compute.h
	Compute.cpp
	DPoolHandler.h
	DPoolHandler.cpp
	BindingInfo.h
	DSetlLayoutFactory.h
	DSetLayoutFactory.cpp
	LoadShaderModule.h
	LoadShaderModule.cpp
)

add_library(PipelineHandler ${SRCS})

target_include_directories(PipelineHandler PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(PipelineHandler PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET PipelineHandler PROPERTY CXX_STANDARD 17)
