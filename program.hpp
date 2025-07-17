#pragma once
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

using std::shared_ptr;

class Program
{
public:
    static shared_ptr<Program> from_shaders(const shared_ptr<const Shader> &shader_vertex,
                                            const shared_ptr<const Shader> &shader_fragment);

    Program(const Program &program) = delete;
    ~Program();

    GLuint id();

protected:
    Program(const shared_ptr<const Shader> &shader_vertex,
            const shared_ptr<const Shader> &shader_fragment);

    GLuint m_id;
    shared_ptr<const Shader> m_shader_vertex;
    shared_ptr<const Shader> m_shader_fragment;
};