#pragma once
#include <cstddef>
#include <memory>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// FIXME paths
#include "../program.hpp"
#include "../vbo.hpp"

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

        Attributes(const vec2 &position);
        static constexpr size_t num_floats();
    };

    Sky(const Sky &another) = delete;
    Sky(const shared_ptr<const Program> &program);

    const shared_ptr<const VertexBufferObject> vbo() const;
    void set_camera_pitch(const float pitch) const;

protected:
    const shared_ptr<const VertexBufferObject> m_vbo;
    const shared_ptr<const Program> m_program;

    GLint m_a_position;
    GLint m_u_camera_pitch;
};

}