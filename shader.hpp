#pragma once
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;

class Shader
{
public:
    static Shader from_file(const string &path, GLenum type);
    static Shader from_string(const string &source, GLenum type);

    Shader(const Shader &) = delete;
    ~Shader();

    GLenum type() const;
    GLuint id() const;

protected:
    Shader(const string &source, GLenum type);

    GLuint m_type;
    GLuint m_id;
};