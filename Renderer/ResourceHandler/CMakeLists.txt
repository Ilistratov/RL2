cmake_minimum_required (VERSION 3.8)

set(SRCS 
	CommandPool.h
	CommandPool.cpp
	BufferBase.h
	BufferBase.cpp
	StructuredBuffer.h
	StructuredBuffer.cpp
	ImageBase.h
	ImageBase.cpp
	RenderTarget.h
	RenderTarget.cpp
)

add_library(ResourceHandler ${SRCS})

target_include_directories(ResourceHandler PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(ResourceHandler PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET ResourceHandler PROPERTY CXX_STANDARD 17)
