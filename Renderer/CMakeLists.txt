cmake_minimum_required (VERSION 3.8)

add_subdirectory(ResourceHandler)
add_subdirectory(PipelineHandler)

set(SRCS 
	DataComponent/BufferData.h
	DataComponent/ImageData.h
	DataComponent/CommandPoolData.h
	ApiBase.h
	ApiBase.cpp
	Core.h
	Core.cpp
	Swapchain.h
	Swapchain.cpp
)

add_library(Renderer ${SRCS})

set_property(TARGET Renderer PROPERTY CXX_STANDARD 17)

add_dependencies(Renderer ResourceHandler)
add_dependencies(Renderer PipelineHandler)

target_link_libraries(Renderer ResourceHandler)
target_link_libraries(Renderer PipelineHandler)

target_include_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Renderer PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

target_link_directories(Renderer PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/lib-x64)
target_link_directories(Renderer PUBLIC $ENV{VULKAN_SDK}/Lib)

target_link_libraries(Renderer vulkan-1)
target_link_libraries(Renderer SDL2)
target_link_libraries(Renderer SDL2main)