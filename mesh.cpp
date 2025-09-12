#include "mesh.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <glm/glm.hpp>

Mesh::Mesh(const std::vector<glm::vec3> &positions,
           const std::vector<glm::vec2> &textures,
           const std::vector<glm::vec3> &normals,
           const std::vector<int> &positions_refs,
           const std::vector<int> &textures_refs,
           const std::vector<int> &normals_refs,
           const std::vector<int> &refs_ends,
           const std::vector<int> &groups_ends,
           const std::vector<std::string> &groups_names)
     : positions(positions), textures(textures), normals(normals),
     positions_refs(positions_refs), textures_refs(textures_refs), normals_refs(normals_refs),
     refs_ends(refs_ends), groups_ends(groups_ends), groups_names(groups_names)
{}

Mesh Mesh::triangulate() const
{
    std::vector<int> new_positions_refs;
    std::vector<int> new_textures_refs;
    std::vector<int> new_normals_refs;
    std::vector<int> new_refs_ends = {0};
    std::vector<int> new_groups_ends = {0};

    for (std::size_t i = 0; i < groups_ends.size()-1; ++i) {
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
