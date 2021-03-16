cmake_minimum_required (VERSION 3.8)

add_subdirectory(CmdRecorder)
add_subdirectory(DescriptorHandler)
add_subdirectory(FrameGraph)
add_subdirectory(ResourceHandler)
add_subdirectory(PipelineHandler)
#add_subdirectory(ShaderBindable)
#add_subdirectory(ShaderHandler)

set(SRCS 
	ApiBase.h
	ApiBase.cpp
	Core.h
	Core.cpp
	Swapchain.h
	Swapchain.cpp
#	CmdExecutor.h
#	CmdExecutor.cpp
)

add_library(Renderer ${SRCS})

set_property(TARGET Renderer PROPERTY CXX_STANDARD 17)

add_dependencies(Renderer CmdRecorder)
add_dependencies(Renderer DescriptorHandler)
add_dependencies(Renderer FrameGraph)
add_dependencies(Renderer ResourceHandler)
add_dependencies(Renderer PipelineHandler)
#add_dependencies(Renderer ShaderBindable)
#add_dependencies(Renderer ShaderHandler)

target_link_libraries(Renderer CmdRecorder)
target_link_libraries(Renderer DescriptorHandler)
target_link_libraries(Renderer FrameGraph)
target_link_libraries(Renderer ResourceHandler)
target_link_libraries(Renderer PipelineHandler)
#target_link_libraries(Renderer ShaderBindable)
#target_link_libraries(Renderer ShaderHandler)

target_include_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(Renderer PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/include PUBLIC $ENV{VULKAN_SDK}/Include)

target_link_directories(Renderer PUBLIC ${PROJECT_SOURCE_DIR}/Third-party/lib-x64)
target_link_directories(Renderer PUBLIC $ENV{VULKAN_SDK}/Lib)

target_link_libraries(Renderer vulkan-1)
target_link_libraries(Renderer glfw3)
