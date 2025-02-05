add_library(newpson_parsing_obj_parser STATIC)
target_sources(newpson_parsing_obj_parser
    PRIVATE
    parser.cpp
    PUBLIC
    parser.h
    parser-internal.h)
target_include_directories(newpson_parsing_obj_parser
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(newpson_parsing_obj_parser
    PUBLIC
    newpson_mesh
    Qt${QT_VERSION_MAJOR}::Core)

add_subdirectory(test)
