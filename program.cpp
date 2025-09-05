#include "program.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <memory>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "status.hpp"

using std::runtime_error;
using std::shared_ptr;
using glm::mat4;

shared_ptr<const Program> Program::from_shaders(const shared_ptr<const Shader> &shader_vertex,
                                                const shared_ptr<const Shader> &shader_fragment)
{
    return shared_ptr<const Program>(new Program(shader_vertex, shader_fragment));
}

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

GLint Program::get_attribute_location(const GLchar * const name) const
{
    GLint location = glGetAttribLocation(m_id, name);
    if (location < 0)
        throw runtime_error(std::format(
            "unable to find attribute \"{}\" in program #{}", name, m_id));
    return location;
}

GLint Program::get_uniform_location(const GLchar * const name) const
{
    GLint location = glGetUniformLocation(m_id, name);
    if (location < 0)
        throw runtime_error(std::format(
            "unable to find uniform \"{}\" in program #{}", name, m_id));
    return location;
}

void Program::set_uniform(const GLint location, const float value) const
{
    glUseProgram(m_id);
    glUniform1f(location, value);
}

void Program::set_uniform(const GLint location, const mat4 &matrix) const
{
    glUseProgram(m_id);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::set_uniform(const GLint location, const vec2 &vector) const
{
    glUseProgram(m_id);
    glUniform2f(location, vector.x, vector.y);

}

Program::~Program()
{
    glDeleteProgram(m_id);
}

GLuint Program::id() const
{
    return m_id;
}