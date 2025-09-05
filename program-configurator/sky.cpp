#include "sky.hpp"

#include <GL/gl.h>
#include <GL/glew.h>

namespace ProgramConfigurators {

Sky::Sky(const shared_ptr<const Program> &program)
    : m_program(program),
      m_vbo(VertexBufferObject::create("sky-plane", {
        1.0f, 1.0f,
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f
      }, 6))
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo->id());
    glVertexAttribPointer(m_a_position, vec2::length(), GL_FLOAT,
        GL_FALSE, sizeof(Attributes),
        reinterpret_cast<void *>(offsetof(Attributes, position_x)));
    glEnableVertexAttribArray(m_a_position);
}

const shared_ptr<const VertexBufferObject> Sky::vbo() const
{
    return m_vbo;
}

void Sky::set_camera_pitch(const float pitch) const
{
    m_program->set_uniform(m_u_camera_pitch, pitch);
}

}