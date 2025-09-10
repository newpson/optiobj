cmake_minimum_required(VERSION 3.14)

project(optiobj LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glm REQUIRED)

# set(IMGUI_BUILD_GLFW_BINDING TRUE)
# set(IMGUI_BUILD_OPENGL3_BINDING TRUE)
# add_library(imgui STATIC)
# target_sources(imgui
#     PRIVATE
#     imgui/imgui.cpp
#     imgui/imgui_draw.cpp
#     imgui/imgui_tables.cpp
#     imgui/imgui_widgets.cpp
#     imgui/backends/imgui_impl_glfw.cpp
#     imgui/backends/imgui_impl_opengl3.cpp
#     PUBLIC
#     imgui/imgui.h
#     imgui/backends/imgui_impl_glfw.h
#     imgui/backends/imgui_impl_opengl3.h)
# target_include_directories(imgui
#     PUBLIC
#     ${CMAKE_CURRENT_LIST_DIR}/imgui
#     ${CMAKE_CURRENT_LIST_DIR}/imgui/backends)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(app)
target_sources(app
    PRIVATE
    main.cpp
    shader.cpp
    camera.cpp
    util.cpp
    transformation.cpp
    status.cpp
    program.cpp
    inputprovider.cpp
    mesh.cpp
    parser.cpp
    arraybuffer.hpp)
target_link_libraries(app
    PUBLIC
    glfw
    OpenGL::GL
    GLEW::GLEW)
target_compile_options(app
    PUBLIC
    "-Wall"
    "-Wextra"
    "-Wpedantic"
    "-g"
    "-fsanitize=address")
target_link_options(app
    PUBLIC
    "-fsanitize=address")
