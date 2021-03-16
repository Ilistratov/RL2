cmake_minimum_required (VERSION 3.8)

set(SRCS 
	FrameGraph.h
	FrameGraph.cpp
	LogicalBufferResource.h
	LogicalBufferResource.cpp
)

add_library(FrameGraph ${SRCS})

target_include_directories(FrameGraph PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(FrameGraph PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET FrameGraph PROPERTY CXX_STANDARD 17)
