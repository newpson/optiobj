add_library(Newpson_Mesh STATIC)
target_sources(Newpson_Mesh
    PRIVATE Mesh.cpp
    PUBLIC Mesh.hpp)
target_include_directories(Newpson_Mesh
    PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(Newpson_Mesh
    PUBLIC Qt6::Core Qt6::Gui)
#PUBLIC Qt5::Core Qt5::Gui)

add_subdirectory(parsing/obj)
