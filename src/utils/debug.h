#pragma once
#include <GL/glew.h>
#include <iostream>

namespace Debug
{
    // TASK 2: Add file name and line number parameters
    inline void checkOpenGLErrors(const char *fileName, int lineNum) {
        GLenum error;
        while ( (error = glGetError()) != GL_NO_ERROR ){
            // Task 2: Edit this print statement to be more descriptive
            std::cout << "File " << fileName << ", line " << lineNum << "\n";
            std::cout << error << std::endl;
        }
    }

    // TASK 3: Add a preprocessor directive to automate the writing of this function
    #define checkOpenGLErrors() checkOpenGLErrors(__FILE__, __LINE__)
}
