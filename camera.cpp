#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "camera.hpp"
#include "transformation.hpp"

using glm::vec3;
using glm::mat4;

CameraFPV::CameraFPV(const float fov, const float aspect_ratio, const vec2 &clip_space)
    : m_fov(fov), m_aspect_ratio(aspect_ratio), m_clip_space(clip_space)
{}

void CameraFPV::translate(const vec3 &deltas)
{
    mat4 R = glm::identity<mat4>();
    R = Transformation::rotate(R, rotation);
    vec3 direction = R * vec4(-deltas, 1.0f);
    position.x += direction.x;
    position.z += direction.z;
    position.y += -deltas.y;
}

void CameraFPV::rotate(const vec3 &deltas)
{
    rotation += -deltas;
}

mat4 CameraFPV::perspective() const
{
    return glm::perspective(m_fov, m_aspect_ratio, m_clip_space[0], m_clip_space[1]);
}

mat4 CameraFPV::ortho() const
{
    return glm::ortho(-1.0f, 1.0f, -1.0f/m_aspect_ratio, 1.0f/m_aspect_ratio, 0.0f, 1.0f);
}