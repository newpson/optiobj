#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "transformation.hpp"

Camera::Camera(const float fov, const float aspect_ratio, const glm::vec2 &clip_space)
    : m_fov(fov), m_aspect_ratio(aspect_ratio), m_clip_space(clip_space)
{}

void Camera::translate(const glm::vec3 &deltas)
{
    glm::mat4 R = glm::identity<glm::mat4>();
    R = Transformation::rotate(R, rotation);
    glm::vec3 direction = R * glm::vec4(-deltas, 1.0f);
    position.x += direction.x;
    position.z += direction.z;
    position.y += -deltas.y;
}

void Camera::rotate(const glm::vec3 &deltas)
{
    rotation += -deltas;
}

glm::mat4 Camera::perspective() const
{
    return glm::perspective(m_fov, m_aspect_ratio, m_clip_space[0], m_clip_space[1]);
}

glm::mat4 Camera::ortho() const
{
    return glm::ortho(-1.0f, 1.0f, -1.0f/m_aspect_ratio, 1.0f/m_aspect_ratio, 0.0f, 1.0f);
}
