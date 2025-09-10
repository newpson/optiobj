#include "status.hpp"
#include <cstddef>
#include <GLFW/glfw3.h>

GLStatus::GLStatus(const std::size_t size)
    : m_size(size * sizeof(GLchar)), m_log(new GLchar [size])
{}

GLStatus::~GLStatus()
{
    delete [] m_log;
}

GLchar * GLStatus::writable_log()
{
    return m_log;
}

GLint * GLStatus::writeable_success_int()
{
    return reinterpret_cast<GLint *>(&m_success);
}

bool * GLStatus::writeable_success_bool()
{
    return &m_success;
}

const char * GLStatus::c_str() const
{
    return m_log;
}

bool GLStatus::is_success() const
{
    return m_success;
}

std::size_t GLStatus::size() const
{
    return m_size;
}
