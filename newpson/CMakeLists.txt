add_library(newpson_mesh STATIC)
target_sources(newpson_mesh
    PRIVATE
    mesh.cpp
    PUBLIC
    mesh.h)
target_include_directories(newpson_mesh
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(newpson_mesh
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui)

add_subdirectory(parsing/obj)
add_subdirectory(ui)
