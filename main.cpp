#include <iostream>
#include <format>
#include "parser.hpp"

int main()
{
    try {
        Mesh mesh = load("/home/newpson/downloads/skull.obj");
    
        std::cerr << std::format("Positions ({}):", mesh.positions.size()) << std::endl;
        for (const vec3 &position: mesh.normals)
            std::cerr << std::format("({}, {}, {})", position.x, position.y, position.z) << std::endl;
    
        std::cerr << std::format("Textures ({}):", mesh.textures.size()) << std::endl;
        for (const vec2 &texture: mesh.textures)
            std::cerr << std::format("({}, {})", texture.x, texture.y) << std::endl;

        std::cerr << std::format("Normals ({}):", mesh.normals.size()) << std::endl;
        for (const vec3 &normal: mesh.normals)
            std::cerr << std::format("({}, {}, {})", normal.x, normal.y, normal.z) << std::endl;
    
        std::cerr << std::format("Faces ({}):", mesh.refs_ends.size() - 1) << std::endl;
        for (int i = 0; i < mesh.refs_ends.size() - 1; ++i) {
            std::cerr << "#" << i+1 << std::endl;
            for (int j = mesh.refs_ends[i]; j < mesh.refs_ends[i+1]; ++j)
                std::cerr << std::format("({}/{}/{})", mesh.positions_refs[j], mesh.textures_refs[j], mesh.normals_refs[j]) << std::endl;
        }

        std::cerr << std::format("Groups ({}):", mesh.groups_names.size()) << std::endl;
        for (int i = 0; i < mesh.groups_ends.size() - 1; ++i)
            std::cerr << std::format("#{} \"{}\" from {} to {}", i+1, mesh.groups_names[i], mesh.groups_ends[i], mesh.groups_ends[i+1]) << std::endl;
    } catch (const ParsingError &error) {
        std::cerr << std::format("parsing error: {} at L{} C{}", error.what(), error.line, error.column) << std::endl;
    }
    return 0;
}