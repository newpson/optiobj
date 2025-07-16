#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "transformation.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;

Transformation::Transformation(const vec3 &position, const vec3 &rotation, const vec3 &scale)
    : position(position), rotation(rotation), scale(scale)
{}

mat4 Transformation::rotate(const mat4 &m, const vec3 &angles)
{
    mat4 result = glm::rotate(m, angles.y, up);
    result = glm::rotate(result, angles.x, right);
    result = glm::rotate(result, angles.z, front);
    return result;
}

mat4 Transformation::matrix() const
{
    mat4 result = glm::identity<mat4>();
    result = glm::translate(result, position);
    result = rotate(result, rotation);
    result = glm::scale(result, scale);
    return result;
}