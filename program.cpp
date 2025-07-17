#include "program.hpp"

#include <format>
#include <stdexcept>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "status.hpp"

using std::shared_ptr;
using std::runtime_error;

Program::Program(const shared_ptr<const Shader> &shader_vertex,
            const shared_ptr<const Shader> &shader_fragment)
    : m_shader_vertex(shader_vertex), m_shader_fragment(shader_fragment)
{
    m_id = glCreateProgram();
    if (m_id == 0)
        throw runtime_error("cannot create program handle");

    glAttachShader(m_id, shader_vertex->id());
    glAttachShader(m_id, shader_fragment->id());
    glLinkProgram(m_id);

    InstantStatusStorage status;
    glGetProgramiv(m_id, GL_LINK_STATUS, status.writeable_success_int());
    if (!status.is_success()) {
        glGetProgramInfoLog(m_id, status.size(), NULL, status.writable_log());
        throw runtime_error(std::format(
            "program #{} linkage error:\n{}", m_id, status.c_str()));
    }
}

Program::~Program()
{
    glDeleteProgram(m_id);
}

GLuint Program::id()
{
    return m_id;
}