cmake_minimum_required(VERSION 3.14)

project(optiobj LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm REQUIRED)

set(IMGUI_BUILD_GLFW_BINDING TRUE)
set(IMGUI_BUILD_OPENGL3_BINDING TRUE)
add_library(imgui STATIC)
target_sources(imgui
    PRIVATE
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    PUBLIC
    imgui/imgui.h
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_opengl3.h)
target_include_directories(imgui
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/imgui
    ${CMAKE_CURRENT_LIST_DIR}/imgui/backends)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)


add_executable(app)
target_sources(app
    PRIVATE
    main-gui.cpp
    camera.cpp
    mesh.cpp
    object.cpp
    parser.cpp
    program.cpp
    scene.cpp
    shader.cpp
    status.cpp
    transformation.cpp
    utility.cpp
    vbo.cpp
    program-configurator/default.cpp
    PUBLIC
    camera.hpp
    mesh.hpp
    object.hpp
    parser.hpp
    program.hpp
    renderable.hpp
    scene.hpp
    shader.hpp
    status.hpp
    transformation.hpp
    utility.hpp
    vbo.hpp
    program-configurator/default.hpp)
target_link_libraries(app
    PUBLIC
    imgui
    glfw
    OpenGL::GL
    GLEW::GLEW)
target_compile_options(app
    PUBLIC
    "-fsanitize=address"
    "-g")
target_link_options(app
    PUBLIC
    "-fsanitize=address")


# ---------------------------


add_executable(test)
target_sources(test
    PRIVATE
    camera-test.cpp
    transformation.cpp
    camera.cpp
    PUBLIC
    transformation.hpp
    camera.hpp)
# target_link_libraries(app
#     PUBLIC)
