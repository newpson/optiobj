#include "shader.hpp"
#include <format>
#include <memory>
#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "status.hpp"
#include "util.hpp"

std::shared_ptr<const Shader> Shader::from_file(const std::string &path, GLenum type)
{
    return from_string(file_to_string(path), type);
}

std::shared_ptr<const Shader> Shader::from_string(const std::string &source, GLenum type)
{
    return std::shared_ptr<const Shader>(new Shader(source, type));
}

Shader::Shader(const std::string &source, GLenum type)
    : m_id(glCreateShader(type))
{
    if (m_id == 0)
        throw std::runtime_error("cannot create shader handle");

    const GLchar * const source_cstr = source.c_str();
    glShaderSource(m_id, 1, &source_cstr, NULL);

    GLStatus status;
    glCompileShader(m_id);
    glGetShaderiv(m_id, GL_COMPILE_STATUS, status.writeable_success_int());
    if (!status.is_success()) {
        glGetShaderInfoLog(m_id, sizeof(status.size()), NULL, status.writable_log());
        throw std::runtime_error(std::format(
            "shader #{} compilation error:\n{}", m_id, status.c_str()));
    }
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

GLenum Shader::type() const
{
    return m_type;
}

GLuint Shader::id() const
{
    return m_id;
}
