#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <glm/glm.hpp>

struct Settings {
    std::string volumeFilePath;

    glm::vec3 volumeScaling = glm::vec3(1.f);
    glm::vec3 volumeTranslate = glm::vec3(0.f);

    int volumeResolutionHighRes = 128;
    int volumeResolutionLowRes = 64;

    float stepSize = 0.1f;    // world-space step size of rays

    bool newFineArray = false;    // flag to tell if fine Worley array needs update
    bool newMediumArray = false;  // flag to tell if medium Worley array needs update
    bool newCoarseArray = false;  // flag to tell if coarse Worley array needs update

    double nearPlane = 0.01;
    double farPlane = 100.0;

    // Channel weights: used in the combination of RGBA channels
    glm::vec4 channelWeight_high = glm::vec4(0.25f);
    glm::vec4 channelWeight_low = glm::vec4(0.25f);

    // Different parameters for the 4 different channels
//    int cellsPerAxisFine = 32;
//    int cellsPerAxisMedium = 16;
//    int cellsPerAxisCoarse = 8;

    struct CellsPerAxis_low {
        int cellsPerAxisFine = 32;
        int cellsPerAxisMedium = 16;
        int cellsPerAxisCoarse = 8;
    };
    CellsPerAxis_low cellsPerAxis_low;

    struct CellsPerAxisAll_low {
        CellsPerAxis_low cellsPerAxisR;
        CellsPerAxis_low cellsPerAxisG;
        CellsPerAxis_low cellsPerAxisB;
        CellsPerAxis_low cellsPerAxisA;

    };
    CellsPerAxisAll_low cellsPerAxisAll_low;

    struct CellsPerAxis_high {
        int cellsPerAxisFine = 32;
        int cellsPerAxisMedium = 16;
        int CellsPerAxisCoarse = 8;
    };
    CellsPerAxis_high cellsPerAxis_high;

    struct CellsPerAxisAll_high {
        CellsPerAxis_low cellsPerAxisR;
        CellsPerAxis_low cellsPerAxisG;
        CellsPerAxis_low cellsPerAxisB;
        CellsPerAxis_low cellsPerAxisA;

    };
    CellsPerAxisAll_high cellsPerAxisAll_high;


    float noiseScaling_low = 1.f; // Worley noise scaling
    glm::vec3 noiseTranslate_low = glm::vec3(0.f);
    float densityMult_low = 1.f;  // density multiplier
    float persistence_low = .7f;  // control blending of Worley noise of diff freq's

    float noiseScaling_high = 1.f; // Worley noise scaling
    glm::vec3 noiseTranslate_high = glm::vec3(0.f);
    float densityMult_high = 1.f;  // density multiplier
    float persistence_high = .7f;  // control blending of Worley noise of diff freq's


    bool invertDensity = true;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
