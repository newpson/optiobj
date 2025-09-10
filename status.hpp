#pragma once
#include <cstddef>
#include <GLFW/glfw3.h>

class GLStatus
{
public:
    GLStatus(const std::size_t size = 512);
    GLStatus(const GLStatus &) = delete;
    ~GLStatus();

    GLchar * writable_log();
    int * writeable_success_int();
    bool * writeable_success_bool();

    const char * c_str() const;
    bool is_success() const;
    size_t size() const;

protected:
    const size_t m_size;
    GLchar * const m_log;
    bool m_success = false;
};
