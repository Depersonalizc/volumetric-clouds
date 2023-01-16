#pragma once

#include "qglobal.h"
#include <glm/glm.hpp>
#include <vector>

// DEBUG
#include <iostream>
#include <glm/gtx/string_cast.hpp>

// A Perlin noise generator class.
class Perlin {

private:
    int cellSize, perlinSize, noiseMapSize;
    std::vector<float> gradient;

public:
    Perlin(int cellSize, int noiseMapSize);
    std::vector<float> formGradient();
    float sample2D(float x, float y);
    float dot(int cellX, int cellY, float vx, float vy);
    std::vector<float> formNoiseMap();
    float lerp(float a, float b, float t);
    float curve(float t);
};
