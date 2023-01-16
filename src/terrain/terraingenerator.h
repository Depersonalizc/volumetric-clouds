#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "noise/perlin-zhou.h"

class TerrainGenerator
{
public:
// constructor and deconstructor
    TerrainGenerator();
    ~TerrainGenerator();

// get functions
    int getResolution() { return m_noiseMapSize; };
    float getScaleX() { return m_xScale; };
    float getScaleY() { return m_yScale; };
    std::vector<float> getHeightMap() { return height_data; };
    std::vector<float> getNormalMap() { return normal_data; };
    std::vector<float> getColorMap() { return color_data; };
    std::vector<float> getCoordMap() { return xz_data; };

// update functions
    void setResolution(int res) {  m_noiseMapSize = res; };
    void setMxMy(float x, float y);
    void setTranslation(glm::vec3 trans);

// generator functions
    void generateTerrain();

private:

    int m_cellSize, m_noiseMapSize; // perlin noise related
    float m_xScale;
    float m_yScale;
    glm::vec3 translation;
    std::vector<float> height_data;
    std::vector<float> normal_data;
    std::vector<float> color_data;
    std::vector<float> xz_data;

    glm::vec3 getPosition(int row, int col);
    float getHeight(int row, int col);
    glm::vec3 getNormal(int row, int col);
    glm::vec3 getColor(glm::vec3 normal, glm::vec3 position);

};
