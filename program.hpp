#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"

class Program
{
public:
    static std::shared_ptr<const Program> from_shaders(
        const std::shared_ptr<const Shader> &shader_vertex,
        const std::shared_ptr<const Shader> &shader_fragment);
    Program(const Program &program) = delete;
    ~Program();

    GLuint id() const;
    void bind() const;

    // FIXME absolutely bullshit API, fuck it. does not work at all, bro.
    GLint get_attribute_location(const GLchar *const name) const;

    GLint get_uniform_location(const GLchar *const name) const;

    void set_uniform(const GLint location, const float value) const;
    void set_uniform(const GLint location, const glm::vec2 &matrix) const;
    void set_uniform(const GLint location, const glm::mat4 &matrix) const;

protected:
    Program(const std::shared_ptr<const Shader> &shader_vertex,
            const std::shared_ptr<const Shader> &shader_fragment);

    GLuint m_id;
    std::shared_ptr<const Shader> m_shader_vertex;
    std::shared_ptr<const Shader> m_shader_fragment;
};
