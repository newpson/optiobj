#pragma once
#include <string>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;

class Shader
{
public:
    static std::shared_ptr<const Shader> from_file(const string &path, GLenum type);
    static std::shared_ptr<const Shader> from_string(const string &source, GLenum type);

    Shader(const Shader &) = delete;
    ~Shader();

    GLenum type() const;
    GLuint id() const;

protected:
    Shader(const string &source, GLenum type);

    GLuint m_type;
    GLuint m_id;
};