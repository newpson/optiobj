#include "transformation.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

Transformation::Transformation(const glm::vec3 &position,
                               const glm::vec3 &rotation,
                               const glm::vec3 &scale)
    : position(position),
      rotation(rotation),
      scale(scale)
{}

glm::mat4 Transformation::rotate(const glm::mat4 &m, const glm::vec3 &angles)
{
    glm::mat4 result = glm::rotate(m, angles.y, up);
    result = glm::rotate(result, angles.x, right);
    result = glm::rotate(result, angles.z, front);
    return result;
}

glm::mat4 Transformation::matrix() const
{
    glm::mat4 result = glm::identity<glm::mat4>();
    result = glm::translate(result, position);
    result = rotate(result, rotation);
    result = glm::scale(result, scale);
    return result;
}
