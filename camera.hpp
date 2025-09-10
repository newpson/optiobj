#pragma once
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include "transformation.hpp"

class Camera: protected Transformation
{
public:
    Camera(const float fov = glm::radians(75.0f),
              const float aspect_ratio = 16.0f/9.0f,
              const glm::vec2 &clip_space = glm::vec2(0.1f, 10.0f));
    void translate(const glm::vec3 &deltas);
    void rotate(const glm::vec3 &deltas);
    glm::mat4 perspective() const;
    glm::mat4 ortho() const;
    using Transformation::matrix;

protected:
    const float m_fov;
    const float m_aspect_ratio;
    const glm::vec2 m_clip_space;
};
