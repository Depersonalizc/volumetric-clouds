#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <array>
#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "camera/camera.h"


constexpr float cube[] = {
    .5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f,
    .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f
};

constexpr auto szVec3() { return sizeof(GLfloat) * 3; }
constexpr auto szVec4() { return sizeof(GLfloat) * 4; }

constexpr auto WORLEY_FINE_MAX_CELLS_PER_AXIS = 256;
constexpr auto WORLEY_COARSE_MAX_CELLS_PER_AXIS = 128;
constexpr auto WORLEY_FINE_MAX_POINTS = 256*256*256;
constexpr auto WORLEY_COARSE_MAX_POINTS = 128*128*128;


class MainWindow;

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void volumeChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;


    // added
//    static constexpr float cube[] = {
//        1.f, 1.f, 0.f,
//        0.f, 1.f, 0.f,
//        1.f, 1.f, 1.f,
//        0.f, 1.f, 1.f,
//        0.f, 0.f, 1.f,
//        0.f, 1.f, 0.f,
//        0.f, 0.f, 0.f,
//        1.f, 1.f, 0.f,
//        1.f, 0.f, 0.f,
//        1.f, 1.f, 1.f,
//        1.f, 0.f, 1.f,
//        0.f, 0.f, 1.f,
//        1.f, 0.f, 0.f,
//        0.f, 0.f, 0.f
//    };

    GLuint vbo, vao;
    GLuint ssboWorleyCoarse, ssboWorleyFine;  // shader storage buffer for worley points
//    GLuint volTexture;

    void glClearScreen() const { glClearColor(.5f, .5f, .5f, 1.f); }
    static void glUnbindVBO() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    static void glUnbindVAO() { glBindVertexArray(0); }
    static void glUnbindSSBO() { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }
    static void glUnbind() { glUnbindVBO(); glUnbindVAO(); glUnbindSSBO(); }

    void setUpShader(const char *vertshaderPath, const char *fragShaderPath);
    void setUpVolume();
    void drawVolume();

    GLuint m_shader;                             // Stores id for shader program
    Camera m_camera;
    bool glInitialized = false;

    // Tick Related Variables
    int m_timer;                                 // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                    // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                  // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;  // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    friend class MainWindow;

    // DEBUG
    void printShaderActiveElements() const {
        GLint i;
        GLint count;
        GLint size; // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)
        const GLsizei bufSize = 16; // maximum name length
        GLchar name[bufSize]; // variable name in GLSL
        GLsizei length; // name length
        /* attribs */
        glGetProgramiv(m_shader, GL_ACTIVE_ATTRIBUTES, &count);
        printf("Active Attributes: %d\n", count);
        for (i = 0; i < count; i++) {
            glGetActiveAttrib(m_shader, (GLuint)i, bufSize, &length, &size, &type, name);
            printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
        }
        /* uniforms */
        glGetProgramiv(m_shader, GL_ACTIVE_UNIFORMS, &count);
        printf("#Active Uniforms: %d\n", count);
        for (i = 0; i < count; i++) {
            glGetActiveUniform(m_shader, (GLuint)i, bufSize, &length, &size, &type, name);
            printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
        }
    }

};
