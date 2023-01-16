#include "terraingenerator.h"

#include <cmath>
#include <iostream>
#include <ostream>
#include "glm/glm.hpp"



// Constructor
TerrainGenerator::TerrainGenerator()
{
    m_noiseMapSize = 200 ;
    m_cellSize = 25;
    translation = glm::vec3(0.0, 0.0, 0.0);
    m_xScale = 5.0;
    m_yScale = 2.0;
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
    auto perlinGen = Perlin(m_cellSize, m_noiseMapSize);
    auto noiseMap = perlinGen.formNoiseMap();

//    for (auto &v : noiseMap)
//        v = v / 3;

    // get height map
    height_data = noiseMap;

    // get xz map
    xz_data.reserve(m_xScale * m_noiseMapSize * m_yScale * m_noiseMapSize * 6);
    for(int x = 0; x < m_xScale * m_noiseMapSize; x++) {
        for(int z = 0; z < m_yScale * m_noiseMapSize; z++) {
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

            if (x<m_noiseMapSize && z<m_noiseMapSize) {
                glm::vec3 n1 = getNormal(x1, z1);
                glm::vec3 c1 = getColor(n1, p1);
                addPointToVector(n1, normal_data);
                addPointToVector(c1, color_data);
            }

        }
    }
}


glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    float x = 1.0 * row / m_noiseMapSize ;
    float y = 1.0 * col / m_noiseMapSize ;
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
    int modRow = (row+m_noiseMapSize)%m_noiseMapSize;
    int modCol = (col+m_noiseMapSize)%m_noiseMapSize;
    int index = modRow*m_noiseMapSize + modCol;
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

    // Task 10: compute color as a function of the normal and position
    float y = position[2];
//    std::cout<<y<<std::endl;
//    glm::vec3 baseColor = glm::vec3(0.48, 0.78, 0.70);
//    glm::vec3 baseColor = glm::vec3(0.133, 0.133, 0.543); // green
//    glm::vec3 baseColor = glm::vec3(0.437, 0.437, 0.437); // dark gray
        glm::vec3 baseColor = glm::vec3(0.199, 0.328, 0.238); // dark blue
//    glm::vec3 midColor = glm::vec3(0.36, 0.20, 0.25); // brown
//    glm::vec3 midColor = glm::vec3(0.63, 0.632, 0.634); // gray
    glm::vec3 midColor = glm::vec3(0.473, 0.703, 0.562); // blue
    glm::vec3 topColor = glm::vec3(1.0, 1.0, 1.0);
    float thres1 = -0.12;
    float thres2 = -0.1;
    float thres3 = 0.006;
    if (y < thres1) {
        return baseColor;
    }else if (y < thres2) {
        float a = 1.0/(thres2 - thres1)*(y - thres1);
        return baseColor * (1-a) + midColor * a;
    }else if (y < thres3) {
        float a = 1.0/(thres3 - thres2)*(y - thres2);
        return midColor * (1-a) + topColor * a;

    }else {
        return glm::vec3(1,1,1);
    }
//    if (glm::dot(normal, glm::vec3(0,0,1)) >= 0.6 && y > 0.06) {
//        return glm::vec3(1,1,1);
//    }
    // Return white as placeholder
    return midColor;
}

//// Computes the intensity of Perlin noise at some point
//float TerrainGenerator::computePerlin(float x, float y) {
//    // Task 1: get grid indices (as ints)
//    int x1 = std::floor(x);
//    int x2 = x1 + 1;
//    int y1 = std::floor(y);
//    int y2 = y1 + 1;

//    // Task 2: compute offset vectors
//    glm::vec2 v1 = glm::vec2(x-x1, y-y1);
//    glm::vec2 v2 = glm::vec2(x-x1, y-y2);
//    glm::vec2 v3 = glm::vec2(x-x2, y-y1);
//    glm::vec2 v4 = glm::vec2(x-x2, y-y2);


//    // Task 3: compute the dot product between offset and grid vectors
//    float dot1 = glm::dot(v1, sampleRandomVector(x1,y1));
//    float dot2 = glm::dot(v2, sampleRandomVector(x1,y2));
//    float dot3 = glm::dot(v3, sampleRandomVector(x2,y1));
//    float dot4 = glm::dot(v4, sampleRandomVector(x2,y2));


//    // Task 5: use your interpolation function to produce the final value
//    float val1 = interpolate(dot1, dot3, x-x1);
//    float val2 = interpolate(dot2, dot4, x-x1);
//    float val3 = interpolate(val1, val2, y-y1);

//    return val3;

//}
