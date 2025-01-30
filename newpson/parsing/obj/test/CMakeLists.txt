add_executable(Newpson_Parsing_OBJ_Test_Mesh)
target_sources(Newpson_Parsing_OBJ_Test_Mesh
    PRIVATE main.cpp
    PUBLIC Mesh.h)
target_link_libraries(Newpson_Parsing_OBJ_Test_Mesh
    PRIVATE Newpson_Mesh Newpson_Parsing_OBJ_Parser Qt5::Core Qt5::Test)
