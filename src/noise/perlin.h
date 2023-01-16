//#pragma once

//#include <glm/glm.hpp>
//#include <vector>

//// DEBUG
//#include <iostream>
//#include <glm/gtx/string_cast.hpp>


//// usage:
////    int noiseRes = 2048;
////    int gridRes = 16;
////    int numOctaves = 8;
////    auto perlinGen = Perlin(noiseRes, gridRes, numOctaves);
////    auto noiseMap = perlinGen.generatePerlinNoise2D();
////    for (auto &v : noiseMap)
////        v = (v + 1) / 2;
////    std::vector<uchar> noiseMapU8(noiseRes*noiseRes);
////    for (int i = 0; i < noiseMapU8.size(); i++)
////        noiseMapU8[i] = noiseMap[i] * 255.f;
////    QImage img(noiseMapU8.data(), noiseRes, noiseRes, QImage::Format_Grayscale8);
////    img.save("out.png");


//// A Perlin noise generator class.
//class Perlin {

//private:
//    size_t noiseResolution, gridResolution, numOctaves;

//public:
//    Perlin(size_t noiseResolution, size_t gridResolution, size_t numFrequencies = 1);
//    static std::vector<glm::vec2> createRandomVectors2D(size_t sideLength);
//    static float getPerlinNoiseValue(float u, float v,
//                                     const glm::vec2 &topleft,
//                                     const glm::vec2 &topright,
//                                     const glm::vec2 &bottomleft,
//                                     const glm::vec2 &bottomright);
//    float getPerlinNoiseValue_f(float x_g, float y_g, int frequency,
//                                const std::vector<glm::vec2> &randomVectors);
//    std::vector<float> generatePerlinNoise2D();
//};
