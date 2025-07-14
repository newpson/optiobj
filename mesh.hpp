#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

using std::vector;
using std::string;
using glm::vec3;
using glm::vec2;

struct Mesh
{
    const vector<vec3> positions;
    const vector<vec2> textures;
    const vector<vec3> normals;
    const vector<int> positions_refs;
    const vector<int> textures_refs;
    const vector<int> normals_refs;
    const vector<int> refs_ends;
    const vector<int> groups_ends;
    const vector<string> groups_names;

    Mesh() = default;

    Mesh(const vector<vec3> &positions,
         const vector<vec2> &textures,
         const vector<vec3> &normals,
         const vector<int> &positions_refs,
         const vector<int> &textures_refs,
         const vector<int> &normals_refs,
         const vector<int> &refs_ends = {0},
         const vector<int> &groups_ends = {0},
         const vector<string> &groups_names = {"default"});

    Mesh triangulate() const;
    // TODO maybe create custom chained iterator over groups -> faces -> positions/textures/normals
};
