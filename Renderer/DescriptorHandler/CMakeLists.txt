cmake_minimum_required (VERSION 3.8)

set(SRCS 
	Layout.h
	Layout.cpp
	Pool.h
	Pool.cpp
)

add_library(DescriptorHandler ${SRCS})

target_include_directories(DescriptorHandler PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(DescriptorHandler  PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET DescriptorHandler  PROPERTY CXX_STANDARD 17)
