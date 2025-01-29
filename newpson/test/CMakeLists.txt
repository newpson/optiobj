add_executable(Newpson_Test_Mesh)
target_sources(Newpson_Test_Mesh
        PRIVATE main.cpp
        PUBLIC Mesh.h)
target_link_libraries(Newpson_Test_Mesh
    PRIVATE Newpson_Mesh Qt5::Core Qt5::Test)
