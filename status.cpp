#include "status.hpp"

#include <cstddef>
#include <GLFW/glfw3.h>

using std::size_t;

InstantStatusStorage::InstantStatusStorage(const size_t size)
    : m_size(size * sizeof(GLchar)), m_log(new GLchar [size])
{}

InstantStatusStorage::~InstantStatusStorage()
{
    delete [] m_log;
}

GLchar * InstantStatusStorage::writable_log()
{
    return m_log;
}

GLint * InstantStatusStorage::writeable_success_int()
{
    return reinterpret_cast<GLint *>(&m_success);
}

bool * InstantStatusStorage::writeable_success_bool()
{
    return &m_success;
}

const char * InstantStatusStorage::c_str() const
{
    return m_log;
}

bool InstantStatusStorage::is_success() const
{
    return m_success;
}

size_t InstantStatusStorage::size() const
{
    return m_size;
}