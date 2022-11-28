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

    float noiseScaling = 1.f; // Worley noise scaling
    glm::vec3 noiseTranslate = glm::vec3(0.f);

    float densityMult = 1.f;  // density multiplier
    float persistence = .7f;  // control blending of Worley noise of diff freq's
    float stepSize = 0.1f;    // world-space step size of rays

    int cellsPerAxisFine = 32;
    int cellsPerAxisMedium = 16;
    int cellsPerAxisCoarse = 8;
    bool newFineArray = false;    // flag to tell if fine Worley array needs update
    bool newMediumArray = false;  // flag to tell if medium Worley array needs update
    bool newCoarseArray = false;  // flag to tell if coarse Worley array needs update

    double nearPlane = 0.01;
    double farPlane = 100.0;

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
