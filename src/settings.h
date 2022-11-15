#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string volumeFilePath;
    double intervalLength = 1.0;
    double nearPlane = 1.0;
    double farPlane = 1.0;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
