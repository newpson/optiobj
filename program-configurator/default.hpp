#pragma once
#include <cstddef>
#include <memory>

#include <glm/glm.hpp>

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
using glm::mat4;

namespace ProgramConfigurators {

class Sky
{
public:
    struct Attributes
    {
        GLfloat position_x;
        GLfloat position_y;
        GLfloat position_z;
        GLfloat normal_x;
        GLfloat normal_y;
        GLfloat normal_z;
        GLfloat texture_x;
        GLfloat texture_y;

        Attributes(const vec3 &position, const vec3 &normal, const vec2 &texture);
        void put_into_vector(vector<float> &vec, const size_t start_i);
        static constexpr size_t num_floats();
    };

    Sky(const Sky &another) = delete;
    Sky(const shared_ptr<const Program> &program);

    vector<shared_ptr<const VertexBufferObject>> generate_vbos(const Mesh &mesh);
    void set_mvp(const mat4 &transformation) const;

protected:
    void configure_vbo(const shared_ptr<const VertexBufferObject> &vbo);

    GLint position;
    GLint normal;
    GLint texture_coords;
    GLint mvp;
    GLint camera_direction;
};

}