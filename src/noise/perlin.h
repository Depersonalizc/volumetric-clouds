#pragma once

#include <glm/glm.hpp>
#include <vector>

// DEBUG
#include <iostream>
#include <glm/gtx/string_cast.hpp>


// A Perlin noise generator class.
class Perlin {

private:
    size_t noiseResolution, gridResolution, numFrequencies;

public:
    Perlin(size_t noiseResolution, size_t gridResolution, size_t numFrequencies = 1);
    static std::vector<glm::vec2> createRandomVectors2D(size_t sideLength);
    std::vector<float> generatePerlinNoise2D();
};
