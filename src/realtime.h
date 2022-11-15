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
#include "utils/shaderloader.h"


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
    static constexpr float cube[] = {
        1.f, 1.f, 0.f,
        0.f, 1.f, 0.f,
        1.f, 1.f, 1.f,
        0.f, 1.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 1.f, 1.f,
        1.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        1.f, 0.f, 0.f,
        0.f, 0.f, 0.f
    };
    GLuint vbo, vao;
    GLuint volTexture;
    glm::vec3 volumeDims = glm::vec3(1.f);
    float volumeScale = 1.f;

    void glClearScreen() const { glClearColor(0.f, 0.f, 0.f, 1.f); }
    static void glUnbindVBO() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    static void glUnbindVAO() { glBindVertexArray(0); }
    static void glUnbind() { glUnbindVBO(); glUnbindVAO(); }
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
