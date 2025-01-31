add_library(Newpson_Parsing_OBJ_Parser STATIC)
target_sources(Newpson_Parsing_OBJ_Parser
    PRIVATE Parser.cpp
    PUBLIC Parser.hpp)
target_include_directories(Newpson_Parsing_OBJ_Parser
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(Newpson_Parsing_OBJ_Parser
    PUBLIC Newpson_Mesh Qt6::Core)
#PUBLIC Newpson_Mesh Qt5::Core)

#add_subdirectory(test)
