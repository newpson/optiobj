#pragma once
#include <glm/glm.hpp>

struct Transformation
{
    /*  y|
         |___x
       z/     */
    static constexpr glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    static constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    // ZXY Euler order
    static glm::mat4 rotate(const glm::mat4 &m, const glm::vec3 &angles);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transformation(const glm::vec3 &position = glm::vec3(0.0),
                   const glm::vec3 &rotation = glm::vec3(0.0),
                   const glm::vec3 &scale = glm::vec3(1.0));

    glm::mat4 matrix() const;
};
