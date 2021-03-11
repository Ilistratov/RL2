cmake_minimum_required (VERSION 3.8)

set(SRCS 
	ICmdRecorder.h
	ICmdRecorder.cpp
	BlitRecorder.h
	BlitRecorder.cpp
	ImgBarrierRecorder.h
	ImgBarrierRecorder.cpp
	PushConstantRecorder.h
	PushConstantRecorder.cpp
)

add_library(CmdRecorder ${SRCS})

target_include_directories(CmdRecorder PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(CmdRecorder  PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

set_property(TARGET CmdRecorder  PROPERTY CXX_STANDARD 17)
