cmake_minimum_required (VERSION 3.8)
project ("RL2" VERSION 0.0.3)

add_subdirectory(Renderer)
add_subdirectory(Utill)

add_executable(RenderLib2  "main.cpp")
set_property(TARGET RenderLib2 PROPERTY CXX_STANDARD 17)

add_dependencies(RenderLib2 Renderer)
add_dependencies(RenderLib2 Utill)

target_link_libraries(RenderLib2 Renderer)
target_link_libraries(RenderLib2 Utill)
