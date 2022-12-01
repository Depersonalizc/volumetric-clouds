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


void Realtime::updateWorleyPoints(const WorleyPointsParams &worleyPointsParams) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorley);
    {
        auto worleyPointsFine = Worley::createWorleyPointArray3D(worleyPointsParams.cellsPerAxisFine);
        auto worleyPointsMedium = Worley::createWorleyPointArray3D(worleyPointsParams.cellsPerAxisMedium);
        auto worleyPointsCoarse = Worley::createWorleyPointArray3D(worleyPointsParams.cellsPerAxisCoarse);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, worleyPointsFine.size()*szVec4(), worleyPointsFine.data());
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, WORLEY_MAX_NUM_POINTS*szVec4(), worleyPointsMedium.size()*szVec4(), worleyPointsMedium.data());
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 2*WORLEY_MAX_NUM_POINTS*szVec4(), worleyPointsCoarse.size()*szVec4(), worleyPointsCoarse.data());
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void Realtime::setUpVolume() {
    // VBO & VAO for the proxy cube
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);  // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, szVec3(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // SSBO for Worley points of three frequencies, with enough memory prealloced
    glGenBuffers(1, &ssboWorley);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboWorley);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboWorley);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3*WORLEY_MAX_NUM_POINTS * szVec4(), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Volume textures (high and low res)
    const auto &dimHiRes = settings.hiResNoise.resolution;
    glGenTextures(1, &volumeTexHighRes);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volumeTexHighRes);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dimHiRes, dimHiRes, dimHiRes, 0, GL_RGBA, GL_FLOAT, nullptr);

    const auto &dimLoRes = settings.loResNoise.resolution;
    glGenTextures(1, &volumeTexLowRes);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, volumeTexLowRes);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dimLoRes, dimLoRes, dimLoRes, 0, GL_RGBA, GL_FLOAT, nullptr);
}

void Realtime::drawVolume() {
    glUseProgram(m_shader);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
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
    glDeleteBuffers(1, &ssboWorley);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(m_shader);
    glDeleteProgram(m_worleyShader);
    glDeleteTextures(1, &volumeTexHighRes);
    glDeleteTextures(1, &volumeTexLowRes);

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
    glClearColor(.5f, .5f, .5f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // composite with bg color
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    /* Create shader */
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_worleyShader = ShaderLoader::createComputeShaderProgram(":/resources/shaders/worley.comp");

    /* Set up VBO, VAO, and SSBO */
    setUpVolume();

    /* Set up default camera */
    m_camera = Camera(SceneCameraData(), size().width(), size().height(), settings.nearPlane, settings.farPlane);

    /* Compute worley noise 3D textures */
    glUseProgram(m_worleyShader);
    for (GLuint texSlot : {0, 1}) {  // high and low res volumes
        // pass uniforms
        const auto &noiseParams = texSlot == 0 ? settings.hiResNoise : settings.loResNoise;
        glUniform1f(glGetUniformLocation(m_worleyShader, "persistence"), noiseParams.persistence);
        glUniform1i(glGetUniformLocation(m_worleyShader, "volumeResolution"), noiseParams.resolution);

        const auto &volumeTex = texSlot == 0 ? volumeTexHighRes : volumeTexLowRes;
        glBindImageTexture(0, volumeTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

        for (int channelIdx = 0; channelIdx < 4; channelIdx++) {
            glm::vec4 channelMask(0.f);
            channelMask[channelIdx] = 1.f;
            glUniform4fv(glGetUniformLocation(m_worleyShader, "channelMask"), 1, glm::value_ptr(channelMask));

            const auto &worleyPointsParams = noiseParams.worleyPointsParams[channelIdx];
            updateWorleyPoints(worleyPointsParams);  // generate new worley points into SSBO
            glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisFine"), worleyPointsParams.cellsPerAxisFine);
            glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisMedium"), worleyPointsParams.cellsPerAxisMedium);
            glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisCoarse"), worleyPointsParams.cellsPerAxisCoarse);
            glDispatchCompute(noiseParams.resolution, noiseParams.resolution, noiseParams.resolution);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }
    }

    /* Pass uniforms to default shader */
    glUseProgram(m_shader);
    {
        // Volume
        glUniform3fv(glGetUniformLocation(m_shader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
        glUniform3fv(glGetUniformLocation(m_shader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));
        glUniform1i(glGetUniformLocation(m_shader, "numSteps"), settings.numSteps);
//        glUniform1f(glGetUniformLocation(m_shader, "stepSize"), settings.stepSize);

        // Noise
        glUniform1f(glGetUniformLocation(m_shader, "densityMult"), settings.densityMult);
        glUniform1i(glGetUniformLocation(m_shader, "invertDensity"), settings.invertDensity);
        glUniform1i(glGetUniformLocation(m_shader, "gammaCorrect"), settings.gammaCorrect);
        // hi-res
        glUniform1f(glGetUniformLocation(m_shader , "hiResNoiseScaling"), settings.hiResNoise.scaling);
        glUniform3fv(glGetUniformLocation(m_shader, "hiResNoiseTranslate"), 1, glm::value_ptr(settings.hiResNoise.translate));
        glUniform4fv(glGetUniformLocation(m_shader, "hiResChannelWeights"), 1, glm::value_ptr(settings.hiResNoise.channelWeights));
        glUniform1f(glGetUniformLocation(m_shader , "hiResDensityOffset"), settings.hiResNoise.densityOffset);
        // lo-res
        glUniform1f(glGetUniformLocation(m_shader , "loResNoiseScaling"), settings.loResNoise.scaling);
        glUniform3fv(glGetUniformLocation(m_shader, "loResNoiseTranslate"), 1, glm::value_ptr(settings.loResNoise.translate));
        glUniform4fv(glGetUniformLocation(m_shader, "loResChannelWeights"), 1, glm::value_ptr(settings.loResNoise.channelWeights));
        glUniform1f(glGetUniformLocation(m_shader , "loResDensityWeight"), settings.loResNoise.densityWeight);

        // Camera
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glUniform3fv(glGetUniformLocation(m_shader, "rayOrigWorld"), 1, glm::value_ptr(m_camera.getPos()));

        // Lighting
//        glUniform1i(glGetUniformLocation(m_shader, "numLights"), 0);
        glUniform4fv(glGetUniformLocation(m_shader, "phaseParams"), 1, glm::value_ptr(glm::vec4(0.83f, 0.3f, 0.8f, 0.15f))); // TODO: make it adjustable hyperparameters
    }
    glUseProgram(0);

    std::cout << "checking errors in initializeGL...\n";
    Debug::checkOpenGLErrors();
    std::cout << "checking done\n";

    glInitialized = true;
}

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawVolume();
//    Debug::checkOpenGLErrors();
}

void Realtime::resizeGL(int w, int h) {
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    m_camera.setWidthHeight(w, h);
    if (m_camera.projChanged()) {
        makeCurrent();
        m_camera.updateProjMatrix();  // only recompute proj matrices if clip planes updated
        m_camera.updateProjView();
        glUseProgram(m_shader);  // Pass camera mat (proj * view)
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glUseProgram(0);
    }
}

void Realtime::volumeChanged() {
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!glInitialized) return;  // avoid gl calls before initialization finishes

    makeCurrent();

    glUseProgram(m_shader);
    // Volume
    glUniform3fv(glGetUniformLocation(m_shader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
    glUniform3fv(glGetUniformLocation(m_shader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));
    glUniform1i(glGetUniformLocation(m_shader, "numSteps"), settings.numSteps);
//        glUniform1f(glGetUniformLocation(m_shader, "stepSize"), settings.stepSize);

    // Noise
    glUniform1f(glGetUniformLocation(m_shader, "densityMult"), settings.densityMult);
    glUniform1i(glGetUniformLocation(m_shader, "invertDensity"), settings.invertDensity);
    glUniform1i(glGetUniformLocation(m_shader, "gammaCorrect"), settings.gammaCorrect);
    // hi-res
    glUniform1f(glGetUniformLocation(m_shader , "hiResNoiseScaling"), settings.hiResNoise.scaling);
    glUniform3fv(glGetUniformLocation(m_shader, "hiResNoiseTranslate"), 1, glm::value_ptr(settings.hiResNoise.translate));
    glUniform4fv(glGetUniformLocation(m_shader, "hiResChannelWeights"), 1, glm::value_ptr(settings.hiResNoise.channelWeights));
    glUniform1f(glGetUniformLocation(m_shader , "hiResDensityOffset"), settings.hiResNoise.densityOffset);
    // lo-res
    glUniform1f(glGetUniformLocation(m_shader , "loResNoiseScaling"), settings.loResNoise.scaling);
    glUniform3fv(glGetUniformLocation(m_shader, "loResNoiseTranslate"), 1, glm::value_ptr(settings.loResNoise.translate));
    glUniform4fv(glGetUniformLocation(m_shader, "loResChannelWeights"), 1, glm::value_ptr(settings.loResNoise.channelWeights));
    glUniform1f(glGetUniformLocation(m_shader , "loResDensityWeight"), settings.loResNoise.densityWeight);


    glUseProgram(m_worleyShader);
    // TODO: recompute both hi and lo res textures if persistence changes
    //       recompute either hi or lo if
    //       - resolution, or
    //       - cellsPerAxis____ of any channel changes
    auto newArray = settings.newFineArray || settings.newMediumArray || settings.newCoarseArray;
    if (newArray) {
        if (settings.newFineArray) {
            settings.newFineArray = false;
        }
        if (settings.newMediumArray) {
            settings.newMediumArray = false;
        }
        if (settings.newCoarseArray) {
            settings.newCoarseArray = false;
        }

        for (GLuint texSlot : {0, 1}) {  // high and low res volumes
            // pass uniforms
            const auto &noiseParams = texSlot == 0 ? settings.hiResNoise : settings.loResNoise;
            glUniform1f(glGetUniformLocation(m_worleyShader, "persistence"), noiseParams.persistence);
            glUniform1i(glGetUniformLocation(m_worleyShader, "volumeResolution"), noiseParams.resolution);

            const auto &volumeTex = texSlot == 0 ? volumeTexHighRes : volumeTexLowRes;
            glBindImageTexture(0, volumeTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

            for (int channelIdx = 0; channelIdx < 4; channelIdx++) {
                glm::vec4 channelMask(0.f);
                channelMask[channelIdx] = 1.f;
                glUniform4fv(glGetUniformLocation(m_worleyShader, "channelMask"), 1, glm::value_ptr(channelMask));

                const auto &worleyPointsParams = noiseParams.worleyPointsParams[channelIdx];
                updateWorleyPoints(worleyPointsParams);  // generate new worley points into SSBO
                glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisFine"), worleyPointsParams.cellsPerAxisFine);
                glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisMedium"), worleyPointsParams.cellsPerAxisMedium);
                glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisCoarse"), worleyPointsParams.cellsPerAxisCoarse);
                glDispatchCompute(noiseParams.resolution, noiseParams.resolution, noiseParams.resolution);
                glMemoryBarrier(GL_ALL_BARRIER_BITS);
            }
        }
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
