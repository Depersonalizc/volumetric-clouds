#pragma once

#include <glm/glm.hpp>
#include <vector>

// DEBUG
#include <iostream>
#include <glm/gtx/string_cast.hpp>


// A Worley noise generator class.
class Worley {

public:
    static std::vector<glm::vec2> createWorleyPointArray2D(size_t sideLength);
    static std::vector<glm::vec4> createWorleyPointArray3D(size_t sideLength);
};
