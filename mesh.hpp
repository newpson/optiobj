#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Mesh
{
    const std::vector<glm::vec3> positions;
    const std::vector<glm::vec2> textures;
    const std::vector<glm::vec3> normals;
    const std::vector<int> positions_refs;
    const std::vector<int> textures_refs;
    const std::vector<int> normals_refs;
    const std::vector<int> refs_ends;
    const std::vector<int> groups_ends;
    const std::vector<std::string> groups_names;

    Mesh() = default;

    Mesh(const std::vector<glm::vec3> &positions,
         const std::vector<glm::vec2> &textures,
         const std::vector<glm::vec3> &normals,
         const std::vector<int> &positions_refs,
         const std::vector<int> &textures_refs,
         const std::vector<int> &normals_refs,
         const std::vector<int> &refs_ends = {0},
         const std::vector<int> &groups_ends = {0},
         const std::vector<std::string> &groups_names = {"default"});

    Mesh triangulate() const;
    // TODO maybe create custom chained iterator over groups -> faces -> positions/textures/normals
};
