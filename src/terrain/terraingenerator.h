#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "noise/perlin.h"

class TerrainGenerator
{
public:
    bool m_wireshade;

    TerrainGenerator();
    ~TerrainGenerator();
    int getResolution() { return m_resolution; };
    void setResolution(int res) {
        m_resolution = res;
    }
    void setMxMy(float x, float y);
    void setTranslation(glm::vec3 trans);
    void generateTerrain();

    std::vector<float> height_data;
    std::vector<float> normal_data;
    std::vector<float> color_data;
    std::vector<float> xz_data;

private:

    // Member variables for terrain generation. You will not need to use these directly.
    std::vector<glm::vec2> m_randVecLookup;
    int m_resolution;
    int m_lookupSize;
    float m_xScale = 2.0;
    float m_yScale = 1.0;
    glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);

    // Samples the (infinite) random vector grid at (row, col)
    glm::vec2 sampleRandomVector(int row, int col);

    // Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
    // Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
    glm::vec3 getPosition(int row, int col);

    // ================== Students, please focus on the code below this point

    // Takes a normalized (x, y) position, in range [0,1)
    // Returns a height value, z, by sampling a noise function
    float getHeight(int x, int y);

    // Computes the normal of a vertex by averaging neighbors
    glm::vec3 getNormal(int row, int col);

    // Computes color of vertex using normal and, optionally, position
    glm::vec3 getColor(glm::vec3 normal, glm::vec3 position);

    // Computes the intensity of Perlin noise at some point
    float computePerlin(float x, float y);
};
