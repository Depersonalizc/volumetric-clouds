#include "realtime.h"
#include "settings.h"
#include "noise/worley.h"
#include "utils/shaderloader.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>

#include "utils/debug.h"


void Realtime::setUpShader(const char *vertshaderPath, const char *fragShaderPath) {
    m_shader = ShaderLoader::createShaderProgram(vertshaderPath, fragShaderPath);
}

void Realtime::setUpVolume() {
    makeCurrent();

    // VBO & VAO (Proxy cube)
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, szVec3(), 0);

    // SSBO (Worley points)
    // declare Worley points SSBOs with enough memory prealloced
    glGenBuffers(1, &ssboWorleyFine);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorleyFine);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboWorleyFine);
    // maximum 256^3 vec4 positions
    glBufferData(GL_SHADER_STORAGE_BUFFER, WORLEY_FINE_MAX_POINTS * szVec4(), NULL, GL_STATIC_DRAW);
    auto worleyPointsFine = Worley::createWorleyPointArray3D(settings.cellsPerAxisFine);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, worleyPointsFine.size()*szVec4(), worleyPointsFine.data());

//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &ssboWorleyCoarse);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorleyCoarse);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboWorleyCoarse);
    // maximum 128^3 vec4 positions
    glBufferData(GL_SHADER_STORAGE_BUFFER, WORLEY_COARSE_MAX_POINTS * szVec4(), NULL, GL_STATIC_DRAW);
    auto worleyPointsCoarse = Worley::createWorleyPointArray3D(settings.cellsPerAxisCoarse);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, worleyPointsCoarse.size()*szVec4(), worleyPointsCoarse.data());

    glUnbind();


//    // 3D texture
//    glGenTextures(1, &volTexture);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_3D, volTexture);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_3D, 0);
}

void Realtime::drawVolume() {
    makeCurrent();
    glUseProgram(m_shader);
    glBindVertexArray(vao);
//    glBindTexture(GL_TEXTURE_3D, volTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

//    glBindTexture(GL_TEXTURE_3D, 0);
    glUnbindVAO();
    glUseProgram(0);
}

Realtime::Realtime(QWidget *parent) : QOpenGLWidget(parent) {
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;
    m_keyMap[Qt::Key_Shift]   = false;
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ssboWorleyFine);
    glDeleteBuffers(1, &ssboWorleyCoarse);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(m_shader);

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);  // cull front face
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // composite with bg color
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Set up shader
    setUpShader("resources/shaders/default.vert", "resources/shaders/default.frag");

    /* Set up VBO, VAO, and volume ssbo */
    setUpVolume();

    /* Set up (default) camera */
    m_camera = Camera(SceneCameraData(), size().width(), size().height(), settings.nearPlane, settings.farPlane);

    /* Pass uniforms */
    glUseProgram(m_shader);
    {
        glUniform1f(glGetUniformLocation(m_shader, "stepSize"), settings.stepSize);
        glUniform1f(glGetUniformLocation(m_shader, "densityMult"), settings.densityMult);
        glUniform1i(glGetUniformLocation(m_shader, "invertDensity"), settings.invertDensity);

        glUniform1i(glGetUniformLocation(m_shader, "cellsPerAxisFine"), settings.cellsPerAxisFine);
        glUniform1i(glGetUniformLocation(m_shader, "cellsPerAxisCoarse"), settings.cellsPerAxisCoarse);
//        glUniform1i(glGetUniformLocation(m_shader, "numPointsFine"),
//                    settings.cellsPerAxisFine * settings.cellsPerAxisFine * settings.cellsPerAxisFine);
//        glUniform1i(glGetUniformLocation(m_shader, "numPointsCoarse"),
//                    settings.cellsPerAxisCoarse * settings.cellsPerAxisCoarse * settings.cellsPerAxisCoarse);

        glUniform3fv(glGetUniformLocation(m_shader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
        glUniform3fv(glGetUniformLocation(m_shader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));
        glUniform1f(glGetUniformLocation(m_shader, "noiseScaling"), settings.noiseScaling);
        glUniform3fv(glGetUniformLocation(m_shader, "noiseTranslate"), 1, glm::value_ptr(settings.noiseTranslate));

        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glUniform3fv(glGetUniformLocation(m_shader, "rayOrigWorld"), 1, glm::value_ptr(m_camera.getPos()));

        glUniform1i(glGetUniformLocation(m_shader, "numLights"), 0);
    }
    glUseProgram(0);

    std::cout << "checking errors in initializeGL...\n";
    Debug::checkOpenGLErrors();
    std::cout << "checking done\n";

    glInitialized = true;
}

void Realtime::paintGL() {
    glClearScreen();
    drawVolume();
//    Debug::checkOpenGLErrors();
}

void Realtime::resizeGL(int w, int h) {
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    m_camera.setWidthHeight(w, h);
    if (m_camera.projChanged()) {
        m_camera.updateProjMatrix();  // only recompute proj matrices if clip planes updated
        m_camera.updateProjView();
        makeCurrent();
        glUseProgram(m_shader);  // Pass camera mat (proj * view)
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glUseProgram(0);
    }
}

void Realtime::volumeChanged() {

//    // clear the old volume texture

//    /* load the new volume into the 3d texture */
//    int dimX = 256, dimY = 256, dimZ = 256;
//    int size = dimX * dimY * dimZ;

//    // rescale volume so that longest side has length 1 in world space
//    volumeDims = {dimX, dimY, dimZ};
//    volumeScale = 1.f / glm::compMax(volumeDims);
//    volumeDims *= volumeScale;

//    // read volume data
//    std::ifstream ifs(settings.volumeFilePath, std::ifstream::binary);
//    if (!ifs) {
//        std::cout << "Failed to load data from \"" << settings.volumeFilePath << "\"\n";
//        return;
//    }
//    std::vector<char> volData(size);
//    ifs.read(volData.data(), size);

//    // pass volume data to texture
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_3D, volTexture);
//    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, dimX, dimY, dimZ, 0, GL_RED, GL_UNSIGNED_BYTE, volData.data());
//    glBindTexture(GL_TEXTURE_3D, 0);

//    // pass volumeDims uniform
//    glUseProgram(m_shader);
//    glUniform3fv(glGetUniformLocation(m_shader, "volumeDims"), 1, glm::value_ptr(volumeDims));
//    glUniform1f(glGetUniformLocation(m_shader, "volumeScale"), volumeScale);
//    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
//    m_camera.setNearFarPlanes(settings.nearPlane, settings.farPlane);

    if (!glInitialized) return;  // avoid gl calls before initialization finishes

    glUseProgram(m_shader);  // Pass camera mat (proj * view)

    // update volume rendering params
    glUniform1f(glGetUniformLocation(m_shader, "stepSize"), settings.stepSize);
    glUniform1f(glGetUniformLocation(m_shader, "densityMult"), settings.densityMult);
    glUniform1i(glGetUniformLocation(m_shader, "invertDensity"), settings.invertDensity);

    glUniform1f(glGetUniformLocation(m_shader, "noiseScaling"), settings.noiseScaling);
    glUniform3fv(glGetUniformLocation(m_shader, "noiseTranslate"), 1, glm::value_ptr(settings.noiseTranslate));

    glUniform3fv(glGetUniformLocation(m_shader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
    glUniform3fv(glGetUniformLocation(m_shader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));

    // update and pass new Worley array if needed
    if (settings.newFineArray) {
        glUniform1i(glGetUniformLocation(m_shader, "cellsPerAxisFine"), settings.cellsPerAxisFine);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorleyFine);
        auto worleyPointsFine = Worley::createWorleyPointArray3D(settings.cellsPerAxisFine);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, worleyPointsFine.size()*szVec4(), worleyPointsFine.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        settings.newFineArray = false;
    }
    if (settings.newCoarseArray) {
        glUniform1i(glGetUniformLocation(m_shader, "cellsPerAxisCoarse"), settings.cellsPerAxisCoarse);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorleyCoarse);
        auto worleyPointsCoarse = Worley::createWorleyPointArray3D(settings.cellsPerAxisCoarse);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, worleyPointsCoarse.size()*szVec4(), worleyPointsCoarse.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        settings.newCoarseArray = false;
    }

    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}

// Camera motion

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        constexpr double sensitivity = 0.5;
        auto deltaTheta = (sensitivity * deltaY) / width();
        auto deltaPhi = -(sensitivity * deltaX) / height();

        auto newTheta = m_camera.getTheta() + deltaTheta;
        auto newPhi = m_camera.getPhi() + deltaPhi;
        m_camera.setThetaPhi(newTheta, newPhi);
        m_camera.updateLook();
        m_camera.updateViewMatrix();
        m_camera.updateProjView();

        makeCurrent();
        glUseProgram(m_shader);  // Pass camera mat (proj * view)
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));        
        glUseProgram(0);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    if (m_keyMap[Qt::Key_Shift])
        deltaTime *= 2;

    float goFront = 0.f;
    float goRight = 0.f;
    float goUp = 0.f;

    // accumulate displacement
    if (m_keyMap[Qt::Key_W])
        goFront += deltaTime;
    if (m_keyMap[Qt::Key_S])
        goFront -= deltaTime;
    if (m_keyMap[Qt::Key_D])
        goRight += deltaTime;
    if (m_keyMap[Qt::Key_A])
        goRight -= deltaTime;
    if (m_keyMap[Qt::Key_Space])
        goUp += deltaTime;
    if (m_keyMap[Qt::Key_Control])
        goUp -= deltaTime;

    auto newCamPos = m_camera.getPos() + goFront * m_camera.getLook()
                                       + goRight * m_camera.getRight()
                                       + goUp    * m_camera.getUp();

    m_camera.setPos(newCamPos);
    m_camera.updateViewMatrix();
    m_camera.updateProjView();
    makeCurrent();
    glUseProgram(m_shader);  // Pass camera uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
    glUniform3fv(glGetUniformLocation(m_shader, "rayOrigWorld"), 1, glm::value_ptr(m_camera.getPos()));
    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}
