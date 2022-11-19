#pragma once

#include <glm/glm.hpp>
#include <vector>

// DEBUG
#include <iostream>
#include <glm/gtx/string_cast.hpp>


// A class representing a virtual camera.
class Worley {

public:
//    Camera() = default;
//    Camera(SceneCameraData const &c_data, int c_width, int c_height,
//           double c_near = 0.1, double c_far = 10000.0);
    static std::vector<glm::vec2> createWorleyPointArray2D(size_t sideLength);
    static std::vector<glm::vec4> createWorleyPointArray3D(size_t sideLength);
};
