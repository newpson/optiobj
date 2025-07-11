// #include <stdexcept>
#include "mesh.hpp"

using glm::vec3;

Mesh::Mesh(const vector<vec3> &positions,
     const vector<vec2> &textures,
     const vector<vec3> &normals,
     const vector<int> &positions_refs,
     const vector<int> &textures_refs,
     const vector<int> &normals_refs,
     const vector<int> &refs_ends,
     const vector<int> &groups_ends,
     const vector<string> &groups_names)
     : positions(positions), textures(textures), normals(normals),
     positions_refs(positions_refs), textures_refs(textures_refs), normals_refs(normals_refs),
     refs_ends(refs_ends), groups_ends(groups_ends), groups_names(groups_names)
{
    // if (!is_consistent())
    //     throw std::runtime_error("Inconsistent"); // TODO custom exception
}