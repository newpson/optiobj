add_executable(Newpson_Parsing_OBJ_Test_Parser)
target_sources(Newpson_Parsing_OBJ_Test_Parser
    PRIVATE main.cpp
    PUBLIC Parser.h)
target_link_libraries(Newpson_Parsing_OBJ_Test_Parser
    PRIVATE Newpson_Mesh Newpson_Parsing_OBJ_Parser Qt5::Core Qt5::Test)
