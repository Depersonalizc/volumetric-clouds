#include "perlin-zhou.h"
#include <random>
#include <chrono>

Perlin::Perlin(int cellSize, int noiseMapSize) :
    cellSize(cellSize), noiseMapSize(noiseMapSize) {
    perlinSize = std::ceil(noiseMapSize/cellSize);
    gradient = formGradient();
}

float Perlin::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Perlin::curve(float t) {
    return t * t * (3 - 2 * t);
}

std::vector<float> Perlin::formGradient() {
    int length = perlinSize * perlinSize * 2;
    std::vector<float> gradient(length);

    std::mt19937_64 rng;
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> U(0, glm::two_pi<float>());

    for (int i = 0; i < length; i += 2) {
        float theta = U(rng);
        float x = std::sin(theta);
        float y = std::cos(theta);
        gradient[i] = x;
        gradient[i+1] = y;
    }

    return gradient;
}

float Perlin::dot(int cellX, int cellY, float vx, float vy) {
    int offset = (cellX + cellY * perlinSize) * 2;
    float wx = gradient[offset];
    float wy = gradient[offset+1];
    return wx*vx + wy*vy;
}

float Perlin::sample2D(float x, float y) {
    int cellX = std::floor(x);
    int cellY = std::floor(y);
    float fracX = x - cellX;
    float fracY = y - cellY;

    int x0 = cellX;
    int y0 = cellY;
    int x1, y1;
    if (cellX == perlinSize-1) {
        x1 = 0;
    } else {
        x1 = cellX + 1;
    }

    if (cellY == perlinSize-1) {
        y1 = 0;
    } else {
        y1 = cellY + 1;
    }

    float v00 = dot(x0, y0, fracX, fracY);
    float v10 = dot(x1, y0, fracX - 1, fracY);
    float v01 = dot(x0, y1, fracX, fracY - 1);
    float v11 = dot(x1, y1, fracX - 1, fracY - 1);

    float vx0 = lerp(v00, v10, curve(fracX));
    float vx1 = lerp(v01, v11, curve(fracX));

    return lerp(vx0, vx1, curve(fracY));
}


std::vector<float> Perlin::formNoiseMap() {
    std::vector<float> noiseMap(noiseMapSize*noiseMapSize);
    for (int r = 0; r < noiseMapSize; r++) {
        for (int c = 0; c < noiseMapSize; c++) {
            auto curVal = sample2D(1.0*r/cellSize, 1.0*c/cellSize);
//            curVal = (curVal + 1) / 2 * 255.f; // convert to image
            noiseMap[r*noiseMapSize+c] = curVal;
        }
    }
    return noiseMap;
}

