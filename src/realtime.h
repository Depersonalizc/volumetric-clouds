#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
//#include "qopenglshaderprogram.h"


#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GL/glew.h>


//#include "GL/glew.h"

#include <glm/glm.hpp>

#include <array>
#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "camera/camera.h"
#include "settings.h"
#include "qopenglshaderprogram.h"

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include "terrain/terraingenerator.h"
#include <QMatrix4x4>

#include "src/glStructure/FBO.h"



constexpr std::array<GLfloat, 42> cube = {
    .5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f,
    .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f
};
constexpr std::array<GLfloat, 30> screenQuadData {
    // POSITION          // UV
    -1.0f,  1.0f, 0.f,  0.f, 1.f,
    -1.0f, -1.0f, 0.f,  0.f, 0.f,
     1.0f, -1.0f, 0.f,  1.f, 0.f,
     1.0f,  1.0f, 0.f,  1.f, 1.f,
    -1.0f,  1.0f, 0.f,  0.f, 1.f,
     1.0f, -1.0f, 0.f,  1.f, 0.f,
};
constexpr auto szVec3() { return sizeof(GLfloat) * 3; }
constexpr auto szVec4() { return sizeof(GLfloat) * 4; }

constexpr auto WORLEY_MAX_CELLS_PER_AXIS = 32;
constexpr auto WORLEY_MAX_NUM_POINTS = WORLEY_MAX_CELLS_PER_AXIS * WORLEY_MAX_CELLS_PER_AXIS * WORLEY_MAX_CELLS_PER_AXIS;


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
//    void wheelEvent(QWheelEvent *event) override;
    
    static void glUnbindVBO() { __glewBindBuffer(GL_ARRAY_BUFFER, 0); }
    static void glUnbindVAO() { __glewBindVertexArray(0); }
    static void glUnbindSSBO() { __glewBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }
    static void glUnbind() { glUnbindVBO(); glUnbindVAO(); glUnbindSSBO(); }

    void updateWorleyPoints(const WorleyPointsParams &worleyPointsParams);
    void setUpTextures();
    void setUpVolume();
    void drawVolume();
    void setUpTerrain();
    void drawTerrain();
    void paintTerrainTexture();
    void rebuildMatrices();
    void setUpScreenQuad();

    GLuint vboScreenQuad, vaoScreenQuad;
    GLuint ssboWorley;
    GLuint volumeTexHighRes, volumeTexLowRes;
    GLuint m_volumeShader, m_worleyShader, m_terrainShader, m_terrainTextureShader;       // Stores id for shader programs

    GLuint sunTexture;
    GLuint nightTexture;

    Camera m_camera;
    bool glInitialized = false;
        GLuint vboVolume, vaoVolume;

    // Tick Related Variables
    int m_timer;                                 // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                    // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                  // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;  // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // Terrain
    std::vector<float> m_terrain_data;
    GLuint m_terrain_vbo;
    GLuint m_terrain_vao;

    glm::mat4 m_proj;
    glm::mat4 m_projView;

    glm::mat4 m_terrain_camera;
    glm::mat4 m_world;

    TerrainGenerator m_terrain;

    QPoint m_prevMousePos;
    float m_angleX;
    float m_angleY;
    float m_zoom;

    std::unique_ptr<FBO> m_FBO;
    GLuint m_terrain_height_texture;
    GLuint m_terrain_normal_texture;
    GLuint m_terrain_color_texture;
    void paintTexture(GLuint texture);
    int m_screen_width;
    int m_screen_height;

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
        glGetProgramiv(m_volumeShader, GL_ACTIVE_ATTRIBUTES, &count);
        printf("Active Attributes: %d\n", count);
        for (i = 0; i < count; i++) {
            glGetActiveAttrib(m_volumeShader, (GLuint)i, bufSize, &length, &size, &type, name);
            printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
        }
        /* uniforms */
        glGetProgramiv(m_volumeShader, GL_ACTIVE_UNIFORMS, &count);
        printf("#Active Uniforms: %d\n", count);
        for (i = 0; i < count; i++) {
            glGetActiveUniform(m_volumeShader, (GLuint)i, bufSize, &length, &size, &type, name);
            printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
        }
    }


};
