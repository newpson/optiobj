#pragma once
#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
    static std::shared_ptr<const Shader> from_file(const std::string &path, GLenum type);
    static std::shared_ptr<const Shader> from_string(const std::string &source, GLenum type);

    Shader(const Shader &another) = delete;
    ~Shader();

    GLenum type() const;
    GLuint id() const;

protected:
    Shader(const std::string &source, GLenum type);

    GLuint m_type;
    GLuint m_id;
};
