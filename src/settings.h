#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <glm/glm.hpp>

struct WorleyPointsParams {
    int cellsPerAxisFine;
    int cellsPerAxisMedium;
    int cellsPerAxisCoarse;
};

struct NoiseParams {
    int resolution;                            // texture resolution
    WorleyPointsParams worleyPointsParams[4];  // overlaied frequencies for each of RGBA channel of noise texture
    glm::vec4 scaling;
    glm::vec3 translate;
    glm::vec4 channelWeights;                  // weights to combine RGBA channels
    float persistence;                         // weight to combine fine/medium/coarse Worley noises
    float densityOffset;                       // for low-res shape noise
    float densityWeight;                       // for hi-res detail noise
};

struct LightParams {
    glm::vec3 color;
    glm::vec3 dir;
    glm::vec4 pos;
    int type;
    float longitude;
    float latitude;
};

struct Settings {
    std::string volumeFilePath;

    // Light
    LightParams lightData = {
        .color = glm::vec3(1),
        .dir = glm::vec3(0,1,0),
        .pos = glm::vec4(0),
        .type = 1,
        .longitude = 0.f,
        .latitude = 0.f,
    };

    // Volume
    glm::vec3 volumeScaling = glm::vec3(1.f);
    glm::vec3 volumeTranslate = glm::vec3(0.f);
    int numSteps = 64; // SMALL_DST_SAMPLE_NUM
    float stepSize = 0.1f;    // world-space step size of rays, not used now
    bool gammaCorrect = false;

    // Noise
    float densityMult = 1.f;  // density multiplier
    float cloudLightAbsorptionMult = 0.75f;
    float minLightTransmittance = 0.01f;
    bool invertDensity = true;

    NoiseParams hiResNoise = {
        .resolution = 200,
        .worleyPointsParams = {
            WorleyPointsParams{6, 4, 2},    // R
            WorleyPointsParams{12, 8, 6},   // G
            WorleyPointsParams{24, 12, 8},  // B
            WorleyPointsParams{32, 24, 12}, // A
        },
        .scaling = glm::vec4(1.f),
        .translate = glm::vec3(0.3f),
        .channelWeights = glm::vec4(1.f, 1.f, 1.f, 1.f),
        .persistence = 0.6f,
        .densityOffset = -0.3f,
    };

    NoiseParams loResNoise = {
        .resolution = 64,
        .worleyPointsParams = {
            WorleyPointsParams{6, 4, 2},    // R
            WorleyPointsParams{12, 8, 6},   // G
            WorleyPointsParams{24, 12, 8},  // B
            WorleyPointsParams{32, 24, 12}, // A
        },
        .scaling = glm::vec4(20.f, 0, 0, 0),
        .translate = glm::vec3(.3f),
        .channelWeights = glm::vec4(1.f, 1.f, 1.f, 1.f),
        .persistence = 0.8f,
        .densityWeight = 8.0f,
    };

    bool newFineArray = false;    // flag to tell if fine Worley array needs update
    bool newMediumArray = false;  // flag to tell if medium Worley array needs update
    bool newCoarseArray = false;  // flag to tell if coarse Worley array needs update

    int curSlot, curChannel; // to denote which one changed

    // Camera
    double nearPlane = 0.01;
    double farPlane = 100.0;

    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
