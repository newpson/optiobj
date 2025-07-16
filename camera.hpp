#pragma once
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include "transformation.hpp"

using glm::vec2;
using glm::vec3;
using glm::radians;

class CameraFPV: protected Transformation
{
public:
    CameraFPV(const float fov = radians(75.0f),
              const float aspect_ratio = 16.0f/9.0f,
              const vec2 &clip_space = vec2(0.1f, 10.0f));
    void translate(const vec3 &deltas);
    void rotate(const vec3 &deltas);
    mat4 perspective() const;
    mat4 ortho() const;
    using Transformation::matrix;

protected:
    const float m_fov;
    const float m_aspect_ratio;
    const vec2 m_clip_space;
};