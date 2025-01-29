cmake_minimum_required(VERSION 3.14)

project(qt-optiobj LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Test Widgets)

add_compile_definitions(PROJECT_ASSETS="${CMAKE_CURRENT_LIST_DIR}/assets")

add_subdirectory(newpson)
