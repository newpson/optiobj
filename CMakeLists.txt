cmake_minimum_required(VERSION 3.14)

project(qt-optiobj LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt5 REQUIRED COMPONENTS Core Gui Test)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Test)

add_compile_definitions(PROJECT_ASSETS="${CMAKE_CURRENT_LIST_DIR}/assets")

add_subdirectory(newpson)

add_executable(app)
target_sources(app
    PRIVATE main.cpp)
target_link_libraries(app
    Newpson_Mesh Newpson_Parsing_OBJ_Parser)

message("Root")
message("${PROJECT_SOURCE_DIR}")

