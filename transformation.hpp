#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Transformation
{
    /*  y|
         |___x
       z/     */
    static constexpr vec3 front = vec3(0.0f, 0.0f, 1.0f);
    static constexpr vec3 up = vec3(0.0f, 1.0f, 0.0f);
    static constexpr vec3 right = vec3(1.0f, 0.0f, 0.0f);

    // ZXY Euler order
    static mat4 rotate(const mat4 &m, const vec3 &angles);

    vec3 position;
    vec3 rotation;
    vec3 scale;

    Transformation(const vec3 &position = vec3(0.0),
                   const vec3 &rotation = vec3(0.0),
                   const vec3 &scale = vec3(1.0));

    mat4 matrix() const;
};