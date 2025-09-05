#include "transformation.hpp"
#include "camera.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <format>

using glm::radians;

int main()
{
    Transformation transformation(
        {0.7f, 0.2f, 0.5f},
        {radians(30.0f), radians(60.0f), radians(50.0f)},
        {1.5f, 2.0f, 4.0f}
    );

    const vec3 point = {0.0f, 0.0f, -1.0f};
    const vec3 result = transformation.matrix() * vec4(point, 1.0f);

    // Tested in Blender (ZXY Euler rotation order)
    const vec3 expected(-2.3f, 2.2f, -1.2321);

    const float difference = glm::length(result - expected);
    std::cout << difference << std::endl;

    // CameraFPV camera;
    // const vec3 point = {0.0f, 0.0f, -1.0f};

    // // camera.translate({0.0f, 0.0f, 0.0f});
    // camera.rotate({-0.1f, 0.0f, 0.0f});

    // const vec4 result = camera.matrix() * vec4(point, 1.0f);
    // const vec4 projection = camera.perspective() * result;

    // std::cout << std::format("result = ({}, {}, {}, {})", result.x, result.y, result.z, result.w) << std::endl;
    // std::cout << std::format("projec = ({}, {}, {}, {})", projection.x, projection.y, projection.z, projection.w) << std::endl;
    return 0;
}