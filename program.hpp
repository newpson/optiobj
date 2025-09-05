#pragma once
#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

using glm::vec2;
using glm::mat4;
using std::shared_ptr;
using std::vector;

class Program
{
public:
    static shared_ptr<const Program> from_shaders(const shared_ptr<const Shader> &shader_vertex,
                                                  const shared_ptr<const Shader> &shader_fragment);
    Program(const Program &program) = delete;
    ~Program();

    GLuint id() const;
    GLint get_attribute_location(const GLchar * const name) const;
    GLint get_uniform_location(const GLchar * const name) const;

    void set_uniform(const GLint location, const float value) const;
    void set_uniform(const GLint location, const mat4 &matrix) const;
    void set_uniform(const GLint location, const vec2 &matrix) const;

protected:
    Program(const shared_ptr<const Shader> &shader_vertex,
            const shared_ptr<const Shader> &shader_fragment);

    GLuint m_id;
    shared_ptr<const Shader> m_shader_vertex;
    shared_ptr<const Shader> m_shader_fragment;
};