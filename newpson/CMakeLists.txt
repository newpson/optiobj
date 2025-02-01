add_library(Newpson_Mesh STATIC)
target_sources(Newpson_Mesh
    PRIVATE
    mesh.cpp
    PUBLIC
    mesh.hpp)
target_include_directories(Newpson_Mesh
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Newpson_Mesh
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui)

add_subdirectory(parsing/obj)
