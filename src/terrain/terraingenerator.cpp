#include "terraingenerator.h"

#include <cmath>
#include <iostream>
#include <ostream>
#include "glm/glm.hpp"



// Constructor
TerrainGenerator::TerrainGenerator()
{
    m_resolution = 2048;
    m_gridRes = 16;
    m_numOctaves = 5;
    translation = glm::vec3(0.0, 0.0, 0.0);
    m_xScale = 2.0;
    m_yScale = 1.0;
}

// Destructor
TerrainGenerator::~TerrainGenerator(){}

// Helper for generateTerrain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.z);
    vector.push_back(point.y);
}


// scaling version zhou
void TerrainGenerator::generateTerrain() {
    auto perlinGen = Perlin(m_resolution, m_gridRes, m_numOctaves);
    auto noiseMap = perlinGen.generatePerlinNoise2D();

    for (auto &v : noiseMap)
        v = v / 20;

    // get height map
    height_data = noiseMap;

    // get xz map
    xz_data.reserve(m_xScale * m_resolution * m_yScale * m_resolution * 6);
    for(int x = 0; x < m_xScale * m_resolution; x++) {
        for(int z = 0; z < m_yScale * m_resolution; z++) {
            int x1 = x;
            int z1 = z;
            int x2 = x + 1;
            int z2 = z + 1;

            glm::vec3 p1 = getPosition(x1, z1);
            glm::vec3 p2 = getPosition(x2, z1);
            glm::vec3 p3 = getPosition(x2, z2);
            glm::vec3 p4 = getPosition(x1, z2);

            // push p3: [x2, z2]
            xz_data.push_back(p3.x);
            xz_data.push_back(p3.y);
            // push p2: [x2, z1]
            xz_data.push_back(p2.x);
            xz_data.push_back(p2.y);
            // push p1: [x1, z1]
            xz_data.push_back(p1.x);
            xz_data.push_back(p1.y);

            // push p4: [x1, z2]
            xz_data.push_back(p4.x);
            xz_data.push_back(p4.y);
            // push p3: [x2, z2]
            xz_data.push_back(p3.x);
            xz_data.push_back(p3.y);
            // push p1: [x1, z1]
            xz_data.push_back(p1.x);
            xz_data.push_back(p1.y);

            if (x<m_resolution && z<m_resolution) {
                glm::vec3 n1 = getNormal(x1, z1);
                glm::vec3 c1 = getColor(n1, p1);
                addPointToVector(n1, normal_data);
                addPointToVector(c1, color_data);
            }

        }
    }
}


glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    float x = 1.0 * row / m_resolution ;
    float y = 1.0 * col / m_resolution ;
    float z = getHeight(row, col);
    return glm::vec3(x,y,z);
}


void TerrainGenerator::setMxMy(float x, float y) {
    m_xScale = x;
    m_yScale = y;

}
void TerrainGenerator::setTranslation(glm::vec3 trans) {
    translation = glm::vec3(trans[0], trans[2], trans[1]);
}


float TerrainGenerator::getHeight(int row, int col) {
    int modRow = (row+m_resolution)%m_resolution;
    int modCol = (col+m_resolution)%m_resolution;
    int index = modRow*m_resolution + modCol;
    return height_data[index];
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    glm::vec3 normal = glm::vec3(0, 0, 0);
    std::vector<std::vector<int>> neighborOffsets = { // Counter-clockwise around the vertex
     {-1, -1},
     { 0, -1},
     { 1, -1},
     { 1,  0},
     { 1,  1},
     { 0,  1},
     {-1,  1},
     {-1,  0}
    };
    glm::vec3 V = getPosition(row,col);
    for (int i = 0; i < 8; ++i) {
     int n1RowOffset = neighborOffsets[i][0];
     int n1ColOffset = neighborOffsets[i][1];
     int n2RowOffset = neighborOffsets[(i + 1) % 8][0];
     int n2ColOffset = neighborOffsets[(i + 1) % 8][1];
     glm::vec3 n1 = getPosition(row + n1RowOffset, col + n1ColOffset);
     glm::vec3 n2 = getPosition(row + n2RowOffset, col + n2ColOffset);
     normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);
}

// TODO: change to the other computing methods by using the height and normal
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    return glm::vec3(0,0,0.4);
}
