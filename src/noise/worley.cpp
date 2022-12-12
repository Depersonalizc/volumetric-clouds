#include "worley.h"
#include <random>
#include <chrono>


inline auto pos3DToIndex(auto x, auto y, auto z, auto xDim, auto yDim) {
    return x + xDim * (y + yDim * z);
}

inline auto pos2DToIndex(auto x, auto y, auto xDim) {
    return x + xDim * y;
}

inline glm::vec3 indexToPos3D(auto index, auto xDim, auto yDim) {
    auto [yz, x] = std::div(index, xDim);
    auto [z , y] = std::div(yz   , yDim);
    return {x, y, z};
}

inline glm::vec2 indexToPos2D(auto index, auto xDim) {
    auto [y, x] = std::div(index, xDim);
    return {x, y};
}


/* Generate (cellsPerAxis x cellsPerAxis x cellsPerAxis)
 * stratified random position samples in [0, 1]^3
 */
std::vector<glm::vec4> Worley::createWorleyPointArray3D(size_t cellsPerAxis) {

    // initialize U(0, 1)
    std::mt19937_64 rng;
//    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t timeSeed = 42;
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> U(0, 1);

    std::vector<glm::vec4> arr(cellsPerAxis * cellsPerAxis * cellsPerAxis);
    float cellSize = 1.f / cellsPerAxis;

    for (int z = 0; z < cellsPerAxis; z++) {
        for (int y = 0; y < cellsPerAxis; y++) {
            for (int x = 0; x < cellsPerAxis; x++) {
                glm::vec3 offset(U(rng), U(rng), U(rng));
                auto cellPos = (glm::vec3(x, y, z) + offset) * cellSize;
                auto index = pos3DToIndex(x, y, z, cellsPerAxis, cellsPerAxis);
                // Append 1 for field alignment
                // https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o
                arr[index] = {cellPos, 1.f};
            }
        }
    }

    return arr;
}

/* Generate (cellsPerAxis x cellsPerAxis)
 * stratified random position samples in [0, 1]^2
 */
std::vector<glm::vec2> Worley::createWorleyPointArray2D(size_t cellsPerAxis) {

    // initialize U(0, 1)
    std::mt19937_64 rng;
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<float> U(0, 1);

    std::vector<glm::vec2> arr(cellsPerAxis * cellsPerAxis);
    float cellSize = 1.f / cellsPerAxis;

    for (int y = 0; y < cellsPerAxis; y++) {
        for (int x = 0; x < cellsPerAxis; x++) {
            glm::vec2 offset(U(rng), U(rng));
            auto cellPos = (glm::vec2(x, y) + offset) * cellSize;
            auto index = pos2DToIndex(x, y, cellsPerAxis);
            arr[index] = cellPos;
        }
    }

    return arr;
}
