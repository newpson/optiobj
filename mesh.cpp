// #include <stdexcept>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "mesh.hpp"

using glm::vec3;
using std::vector;
using std::string;

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

Mesh Mesh::triangulate() const
{
    vector<int> new_positions_refs;
    vector<int> new_textures_refs;
    vector<int> new_normals_refs;
    vector<int> new_refs_ends = {0};
    vector<int> new_groups_ends = {0};

    for (int i = 0; i < groups_ends.size()-1; ++i) {
        for (int j = groups_ends[i]; j < groups_ends[i+1]; ++j)
            for (int k = refs_ends[j] + 1; k < refs_ends[j+1] - 1; ++k) {
                new_positions_refs.push_back(positions_refs[refs_ends[j]]);
                new_positions_refs.push_back(positions_refs[k]);
                new_positions_refs.push_back(positions_refs[k + 1]);
                new_textures_refs.push_back(textures_refs[refs_ends[j]]);
                new_textures_refs.push_back(textures_refs[k]);
                new_textures_refs.push_back(textures_refs[k + 1]);
                new_normals_refs.push_back(normals_refs[refs_ends[j]]);
                new_normals_refs.push_back(normals_refs[k]);
                new_normals_refs.push_back(normals_refs[k + 1]);
                new_refs_ends.push_back(new_positions_refs.size());
            }
        new_groups_ends.push_back(new_refs_ends.size() - 1);
    }
    return Mesh(positions, textures, normals,
new_positions_refs, new_textures_refs, new_normals_refs,
     new_refs_ends, new_groups_ends, groups_names);
}