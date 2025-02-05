cmake_minimum_required(VERSION 3.14)

project(qt-optiobj LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Test Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Test Widgets OpenGLWidgets)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_definitions(PROJECT_ASSETS="${CMAKE_CURRENT_LIST_DIR}/assets")

add_subdirectory(newpson)

add_executable(app)
target_sources(app
    PRIVATE
    main.cpp)
target_link_libraries(app
    PUBLIC
    newpson_mesh
    newpson_parsing_obj_parser)

