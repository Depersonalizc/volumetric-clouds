#include "perlin.h"
#include <random>
#include <chrono>


inline auto pos2DToIndex(auto x, auto y, auto xDim) {
    return x + xDim * y;
}

inline glm::vec2 indexToPos2D(auto index, auto xDim) {
    auto [y, x] = std::div(index, xDim);
    return {x, y};
}

Perlin::Perlin(size_t noiseResolution, size_t gridResolution, size_t numFrequencies) :
    noiseResolution(noiseResolution), gridResolution(gridResolution), numFrequencies(numFrequencies)
{}


/* Generate (sideLength x sideLength) random unit vectors in R^2 */
std::vector<glm::vec2> Perlin::createRandomVectors2D(size_t sideLength) {
    // initialize U(0, 2*pi)
    std::mt19937_64 rng;
//    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t timeSeed = 42;
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> U(0, glm::two_pi<float>());

    std::vector<glm::vec2> arr(sideLength * sideLength);

    for (auto &v : arr) {
        float theta = U(rng);
        v = { std::cos(theta), std::sin(theta) };
    }

    return arr;
}

float lerp(float x0, float x1, float lambda) {
    return (1 - lambda) * x0 + lambda * x1;
}

float bilerp(float u, float v, float topleft, float topright, float bottomleft, float bottomright) {
    auto top = lerp(topleft, topright, u);
    auto bottom = lerp(bottomleft, bottomright, u);
    return lerp(top, bottom, v);
}

float getPerlinNoiseValue(float u, float v,
                          glm::vec2 &topleft,
                          glm::vec2 &topright,
                          glm::vec2 &bottomleft,
                          glm::vec2 &bottomright)
{
    float dotTL, dotTR, dotBL, dotBR;
    dotTL = glm::dot(glm::vec2( -u,  -v), topleft);
    dotTR = glm::dot(glm::vec2(1-u,  -v), topright);
    dotBL = glm::dot(glm::vec2( -u, 1-v), bottomleft);
    dotBR = glm::dot(glm::vec2(1-u, 1-v), bottomright);
    return bilerp(u, v, dotTL, dotTR, dotBL, dotBR);
}

std::vector<float> Perlin::generatePerlinNoise2D() {
    const std::vector<glm::vec2> randomVectors = createRandomVectors2D(gridResolution);
    const float scalePixelToGrid = static_cast<float>(gridResolution) / static_cast<float>(noiseResolution);

    std::vector<float> noiseMap(noiseResolution * noiseResolution);
    for (int col_p = 0; col_p < noiseResolution; col_p++) {
        float x_g = std::fmod(scalePixelToGrid * col_p, gridResolution);
        float u = glm::fract(x_g);
        int col_g = static_cast<int>(x_g) % gridResolution;  // col left
        int colp1_g = static_cast<int>(x_g + 1) % gridResolution;  // col right
        for (int row_p = 0; row_p < noiseResolution; row_p++) {
            float y_g = std::fmod(scalePixelToGrid * row_p, gridResolution);
            float v = glm::fract(y_g);
            int row_g = static_cast<int>(y_g) % gridResolution;
            int rowp1_g = static_cast<int>(y_g + 1) % gridResolution;

//            const glm::vec2 center(x_g, y_g);
            glm::vec2 topleft     = randomVectors[pos2DToIndex(col_g,   row_g,   gridResolution)];
            glm::vec2 topright    = randomVectors[pos2DToIndex(colp1_g, row_g,   gridResolution)];
            glm::vec2 bottomleft  = randomVectors[pos2DToIndex(col_g,   rowp1_g, gridResolution)];
            glm::vec2 bottomright = randomVectors[pos2DToIndex(colp1_g, rowp1_g, gridResolution)];

            if (row_g == gridResolution - 1) {
                bottomleft.y += gridResolution;
                bottomright.y += gridResolution;
            }

            if (col_g == gridResolution - 1) {
                topright.x += gridResolution;
                bottomright.x += gridResolution;
            }

            float noiseVal = getPerlinNoiseValue(u, v, topleft, topright, bottomleft, bottomright);

            int index_p = pos2DToIndex(col_p, row_p, noiseResolution);
            noiseMap[index_p] = noiseVal;
        }
    }

    return noiseMap;
}
