#include "default.hpp"

#include <cstddef>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// FIXME paths
#include "../program.hpp"
#include "../vbo.hpp"
#include "../mesh.hpp"

using std::size_t;
using std::shared_ptr;
using std::vector;
using glm::vec2;
using glm::vec3;

namespace ProgramConfigurators {

Sky::Attributes::Attributes(const vec3 &position, const vec3 &normal, const vec2 &texture)
    : position_x(position.x), position_y(position.y), position_z(position.z),
    normal_x(normal.x), normal_y(normal.y), normal_z(normal.z),
    texture_x(texture.x), texture_y(texture.y)
{}

void Sky::Attributes::put_into_vector(vector<float> &vec, const size_t start_i)
{
    vec[start_i + 0] = position_x;
    vec[start_i + 1] = position_y;
    vec[start_i + 2] = position_z;
    vec[start_i + 3] = normal_x;
    vec[start_i + 4] = normal_y;
    vec[start_i + 5] = normal_z;
    vec[start_i + 6] = texture_x;
    vec[start_i + 7] = texture_y;
}

constexpr size_t Sky::Attributes::num_floats()
{
    return sizeof(Attributes)/sizeof(float);
}

Sky::Sky(const shared_ptr<const Program> &program)
    : position(program->get_attribute_location("a_position")),
      normal(program->get_attribute_location("a_normal")),
    //   texture_coords(program->get_attribute_location("a_texture_coords")),
    //   camera_direction(program->get_uniform_location("u_camera_direction")),
      mvp(program->get_uniform_location("u_mvp"))
{}

void Sky::configure_vbo(const shared_ptr<const VertexBufferObject> &vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id());
    glVertexAttribPointer(position, vec3::length(), GL_FLOAT,
        GL_FALSE, sizeof(Attributes),
        reinterpret_cast<void *>(offsetof(Attributes, position_x)));

    glVertexAttribPointer(normal, vec3::length(), GL_FLOAT,
        GL_FALSE, sizeof(Attributes),
        reinterpret_cast<void *>(offsetof(Attributes, normal_x)));

    // glVertexAttribPointer(texture_coords, vec2::length(), GL_FLOAT,
    //     GL_FALSE, sizeof(Attributes),
    //     reinterpret_cast<void *>(offsetof(Attributes, texture_x)));

    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(normal);
    // glEnableVertexAttribArray(texture);
}

vector<shared_ptr<const VertexBufferObject>> Sky::generate_vbos(const Mesh &mesh)
{
    vector<shared_ptr<const VertexBufferObject>> vbos;
    vbos.reserve(mesh.groups_ends.size() - 1);

    for (int i = 0; i < mesh.groups_ends.size()-1; ++i) {
        for (int j = mesh.groups_ends[i]; j < mesh.groups_ends[i+1]; ++j) {
            const int num_faces = mesh.groups_ends[i+1] - mesh.groups_ends[i];
            if (num_faces == 0)
                continue;

            vector<float> raw_data;
            const int num_vertices = num_faces * 3; // TODO FIXME check if Mesh is triangulated
            raw_data.resize(Attributes::num_floats() * num_vertices);

            for (int k = mesh.refs_ends[j], data_i = 0; k < mesh.refs_ends[j+1]; ++k, data_i += Attributes::num_floats()) {
                Attributes(
                    mesh.positions[mesh.positions_refs[k]],
                    mesh.normals[mesh.normals_refs[k]],
                    mesh.textures[mesh.textures_refs[k]]
                ).put_into_vector(raw_data, data_i);
            }
            auto vbo = VertexBufferObject::create(mesh.groups_names[i], raw_data, num_vertices);
            configure_vbo(vbo);
            vbos.push_back(vbo);
        }
    }

    return vbos;
}

void Sky::set_mvp(const mat4 &transformation) const
{
    glUseProgram(3);
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(transformation));
}


}