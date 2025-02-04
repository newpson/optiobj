add_library(Newpson_Parsing_OBJ_Parser STATIC)
target_sources(Newpson_Parsing_OBJ_Parser
    PRIVATE
    parser.cpp
    PUBLIC
    parser.h
    parser-internal.h)
target_include_directories(Newpson_Parsing_OBJ_Parser
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(Newpson_Parsing_OBJ_Parser
    PUBLIC
    Newpson_Mesh
    Qt${QT_VERSION_MAJOR}::Core)

add_subdirectory(test)
