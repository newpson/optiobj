#include "object.hpp"

#include "transformation.hpp"
#include "vbo.hpp"
#include "program.hpp"

using std::shared_ptr;
using std::vector;

Object::Object(const shared_ptr<const Program> &program,
        const vector<shared_ptr<const VertexBufferObject>> &vbos,
        const Transformation &transformation)
    : m_program(program),
        m_vbos(vbos),
        m_transformation(transformation)
{}

void Object::render()
{
    glUseProgram(m_program->id());
    for (const auto vbo: m_vbos) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo->id());
        glDrawArrays(GL_TRIANGLES, 0, vbo->num_indices());
    }
}
