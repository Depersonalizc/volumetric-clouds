#include "realtime.h"
#include "settings.h"
#include "noise/worley.h"
#include "noise/perlin-zhou.h"
#include "utils/shaderloader.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QImage>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <QDir>

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

void Realtime::setUpScreenQuad() {
    glGenBuffers(1, &vboScreenQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboScreenQuad);
    glBufferData(GL_ARRAY_BUFFER, screenQuadData.size()*sizeof(GLfloat), screenQuadData.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &vaoScreenQuad);
    glBindVertexArray(vaoScreenQuad);
    glEnableVertexAttribArray(0);  // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);  // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          reinterpret_cast<void*>(3 * sizeof(GLfloat)));
}

void Realtime::setUpTextures() {

    // Sun color
    // Read sun color texture
    QImage sunTextureImage("./resources/textures/sun_v1.png");
    sunTextureImage.convertTo(QImage::Format_RGB888);
    auto texWidth = sunTextureImage.width();

    // Generate sun color 1D texture
    glGenTextures(1, &sunTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_1D, sunTexture);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage1D	(GL_TEXTURE_1D, 0, GL_RGB32F, texWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, sunTextureImage.bits());
    glBindTexture(GL_TEXTURE_1D, 0);

    // Night sky texture set up
    QImage nightTextureImage("./resources/textures/stars2.png");
    nightTextureImage.convertTo(QImage::Format_RGBA8888);

    // Generate night sky color 2D texture
    glGenTextures(1, &nightTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, nightTexture);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D	(GL_TEXTURE_2D, 0, GL_RGBA, nightTextureImage.width(), nightTextureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nightTextureImage.bits());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::setUpVolume() {
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
    glDisable(GL_DEPTH_TEST);  // disable depth test for volume rendering
    glUseProgram(m_volumeShader);

    // Bind depth texture to slot #2 and color to #3
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_FBO.get()->getFboDepthTexture());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_FBO.get()->getFboColorTexture());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_1D, sunTexture);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, nightTexture);

    // Draw screen quad
    glBindVertexArray(vaoScreenQuad);
    glDrawArrays(GL_TRIANGLES, 0, screenQuadData.size() / 5);

    // Clear things up
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}


void Realtime::drawTerrain() {
    glUseProgram(m_terrainShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, sunTexture);

    glBindVertexArray(m_terrain_vao);
    int res = m_terrain.getResolution();
    glDrawArrays(GL_TRIANGLES, 0, res*res*6 * m_terrain.getScaleX() * m_terrain.getScaleY());
    glBindVertexArray(0);
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

    glDeleteBuffers(1, &vboVolume);
    glDeleteBuffers(1, &vboScreenQuad);
    glDeleteBuffers(1, &ssboWorley);
    glDeleteVertexArrays(1, &vaoVolume);
    glDeleteVertexArrays(1, &vaoScreenQuad);
    glDeleteProgram(m_volumeShader);
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

    glClearColor(.5f, .5f, .5f, 1.f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    /* Create shader */
    m_volumeShader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_worleyShader = ShaderLoader::createComputeShaderProgram(":/resources/shaders/worley.comp");
    m_terrainShader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain_generator.vert", ":/resources/shaders/terrain_generator.frag");
    m_terrainTextureShader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain_texture.vert", ":/resources/shaders/terrain_texture.frag");

    /* Set up VBO, VAO, and SSBO */
    setUpScreenQuad();
    setUpVolume();

    /* Read and set up textures */
    setUpTextures();

    /* Set up default camera */
    m_camera = Camera(SceneCameraData(), size().width(), size().height(), settings.nearPlane, settings.farPlane);


    /* Set up terrain shader and terrain data */
    glUseProgram(m_terrainShader);
    {
        // VAO, VBO
        setUpTerrain();

        m_world = glm::mat4(1.f);
        m_world = glm::translate(m_world, glm::vec3(-0.5, -0.5, 0));
        //m_world = glm::scale(m_world, glm::vec3(2, 2, 2));

        glm::mat4 projView = m_camera.getProjMatrix() * m_camera.getViewMatrix() * m_world;
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projViewMatrix"), 1, GL_FALSE, glm::value_ptr(projView));

        glm::mat4 transInv = glm::transpose(glm::inverse(m_camera.getViewMatrix() * m_world));
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "transInvViewMatrix"), 1, GL_FALSE, glm::value_ptr(transInv));

        glUniform1f(glGetUniformLocation(m_terrainShader , "testLight.longitude"), settings.lightData.longitude);
        glUniform1f(glGetUniformLocation(m_terrainShader , "testLight.latitude"), settings.lightData.latitude);
        glUniform1i(glGetUniformLocation(m_terrainShader , "testLight.type"), settings.lightData.type);
        glUniform3fv(glGetUniformLocation(m_terrainShader , "testLight.dir"), 1, glm::value_ptr(settings.lightData.dir));
        glUniform3fv(glGetUniformLocation(m_terrainShader , "testLight.color"), 1, glm::value_ptr(settings.lightData.color));
        glUniform4fv(glGetUniformLocation(m_terrainShader , "testLight.pos"), 1, glm::value_ptr(settings.lightData.pos));

        GLint color_texture_loc = glGetUniformLocation(m_terrainShader, "color_sampler");
        glUniform1i(color_texture_loc, 3);
        GLint height_texture_loc = glGetUniformLocation(m_terrainShader, "height_sampler");
        glUniform1i(height_texture_loc, 6);
        GLint normal_texture_loc = glGetUniformLocation(m_terrainShader, "normal_sampler");
        glUniform1i(normal_texture_loc, 7);

        // start height map modification
        glGenTextures(1, &m_terrain_height_texture);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, m_terrain_height_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_terrain.getResolution(), m_terrain.getResolution(), 0, GL_RED, GL_FLOAT, m_terrain.getHeightMap().data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // start normal map modification
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, m_terrain_normal_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_terrain.getResolution(), m_terrain.getResolution(), 0, GL_RGB, GL_FLOAT, m_terrain.getNormalMap().data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // start color map modification
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_terrain_color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_terrain.getResolution(), m_terrain.getResolution(), 0, GL_RGB, GL_FLOAT, m_terrain.getColorMap().data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        // end height map modification
    }

//    glUseProgram(m_terrainTextureShader);
//    GLint depth_texture_loc = glGetUniformLocation(m_terrainTextureShader, "depth_sampler");
//    glUniform1i(depth_texture_loc, 2);
//    GLint color_texture_loc = glGetUniformLocation(m_terrainTextureShader, "color_sampler");
//    glUniform1i(color_texture_loc, 3);

//    glUniform1f(glGetUniformLocation(m_terrainTextureShader, "near"), settings.nearPlane);
//    glUniform1f(glGetUniformLocation(m_terrainTextureShader, "far"), settings.farPlane);

    glUseProgram(0);


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

    /* Pass uniforms to volume shader */
    glUseProgram(m_volumeShader);
    {
        // Volume
        glUniform3fv(glGetUniformLocation(m_volumeShader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
        glUniform3fv(glGetUniformLocation(m_volumeShader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));
        glUniform1i(glGetUniformLocation(m_volumeShader, "numSteps"), settings.numSteps);
//        glUniform1f(glGetUniformLocation(m_volumeShader, "stepSize"), settings.stepSize);

        // Noise
        glUniform1f(glGetUniformLocation(m_volumeShader, "densityMult"), settings.densityMult);
        glUniform1i(glGetUniformLocation(m_volumeShader, "invertDensity"), settings.invertDensity);
        glUniform1i(glGetUniformLocation(m_volumeShader, "gammaCorrect"), settings.gammaCorrect);
        // hi-res
        glUniform4fv(glGetUniformLocation(m_volumeShader , "hiResNoiseScaling"), 1, glm::value_ptr(settings.hiResNoise.scaling));
        glUniform3fv(glGetUniformLocation(m_volumeShader, "hiResNoiseTranslate"), 1, glm::value_ptr(settings.hiResNoise.translate));
        glUniform4fv(glGetUniformLocation(m_volumeShader, "hiResChannelWeights"), 1, glm::value_ptr(settings.hiResNoise.channelWeights));
        glUniform1f(glGetUniformLocation(m_volumeShader , "hiResDensityOffset"), settings.hiResNoise.densityOffset);
        // lo-res
        glUniform1f(glGetUniformLocation(m_volumeShader , "loResNoiseScaling"), settings.loResNoise.scaling[0]);
        glUniform3fv(glGetUniformLocation(m_volumeShader, "loResNoiseTranslate"), 1, glm::value_ptr(settings.loResNoise.translate));
        glUniform4fv(glGetUniformLocation(m_volumeShader, "loResChannelWeights"), 1, glm::value_ptr(settings.loResNoise.channelWeights));
        glUniform1f(glGetUniformLocation(m_volumeShader , "loResDensityWeight"), settings.loResNoise.densityWeight);

        // Camera
        glUniform1f(glGetUniformLocation(m_volumeShader , "xMax"), m_camera.xMax());
        glUniform1f(glGetUniformLocation(m_volumeShader , "yMax"), m_camera.yMax());
        glUniform3fv(glGetUniformLocation(m_volumeShader, "rayOrigWorld"), 1, glm::value_ptr(m_camera.getPos()));
        glUniformMatrix4fv(glGetUniformLocation(m_volumeShader, "viewInverse"), 1, GL_FALSE, glm::value_ptr(m_camera.getViewMatrixInverse()));

        // Lighting
//        glUniform1i(glGetUniformLocation(m_volumeShader, "numLights"), 0);
        glUniform4fv(glGetUniformLocation(m_volumeShader, "phaseParams"), 1, glm::value_ptr(glm::vec4(0.83f, 0.3f, 0.8f, 0.15f))); // TODO: make it adjustable hyperparameters
        glUniform1f(glGetUniformLocation(m_volumeShader , "testLight.longitude"), settings.lightData.longitude);
        glUniform1f(glGetUniformLocation(m_volumeShader , "testLight.latitude"), settings.lightData.latitude);
        glUniform1i(glGetUniformLocation(m_volumeShader , "testLight.type"), settings.lightData.type);
        glUniform3fv(glGetUniformLocation(m_volumeShader , "testLight.dir"), 1, glm::value_ptr(settings.lightData.dir));
        glUniform3fv(glGetUniformLocation(m_volumeShader , "testLight.color"), 1, glm::value_ptr(settings.lightData.color));
        glUniform4fv(glGetUniformLocation(m_volumeShader , "testLight.pos"), 1, glm::value_ptr(settings.lightData.pos));
        glUniform1i(glGetUniformLocation(m_volumeShader, "nightColor"), 5);
        glUniform1i(glGetUniformLocation(m_volumeShader, "sunGradient"), 4);
        glUniform1i(glGetUniformLocation(m_volumeShader, "solidDepth"), 2);
        glUniform1i(glGetUniformLocation(m_volumeShader, "solidColor"), 3);
        glUniform1f(glGetUniformLocation(m_volumeShader, "near"), settings.nearPlane);
        glUniform1f(glGetUniformLocation(m_volumeShader, "far"), settings.farPlane);
    }
    glUseProgram(0);

    // init FBO
    m_FBO = std::make_unique<FBO>(2, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_FBO.get()->makeFBO();

    std::cout << "checking errors in initializeGL...\n";
    Debug::checkOpenGLErrors();
    std::cout << "checking done\n";

    glInitialized = true;
}

void Realtime::setUpTerrain() {
    // Generate and bind the VBO
    glGenBuffers(1, &m_terrain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrain_vbo);

    // Put data into the VBO
    m_terrain.generateTerrain();
    glBufferData(GL_ARRAY_BUFFER,
                 m_terrain.getCoordMap().size() * sizeof(GLfloat),
                 m_terrain.getCoordMap().data(),
                 GL_STATIC_DRAW);

    // Generate and bind the VAO, with our VBO currently bound
    glGenVertexArrays(1, &m_terrain_vao);
    glBindVertexArray(m_terrain_vao);

    // Define VAO attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                             nullptr);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::paintGL() {
    // Render terrain color and depth to FBO textures
//    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO.get()->getFbo());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_terrain_normal_texture);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_terrain_height_texture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_terrain_color_texture);
    drawTerrain();

   // Draw on main screen
   glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
   drawVolume();

    // Clear things up
    glUseProgram(0);
//    Debug::checkOpenGLErrors();
}

void Realtime::paintTerrainTexture() {
    // binding depth to slot #2 and color to #3
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_FBO.get()->getFboDepthTexture());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_FBO.get()->getFboColorTexture());

    glUseProgram(m_terrainTextureShader);
    glBindVertexArray(m_FBO.get()->getFullscreenVao());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void Realtime::resizeGL(int w, int h) {
    makeCurrent();

    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    m_camera.setWidthHeight(w, h);

    if (m_camera.projChanged()) {
        m_camera.updateProjMatrix();  // only recompute proj matrices if clip planes updated
        m_camera.updateProjView();
        glUseProgram(m_terrainShader);  // Pass camera mat (proj * view)
//        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glm::mat4 projView = m_camera.getProjMatrix() * m_camera.getViewMatrix() * m_world;
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projViewMatrix"), 1, GL_FALSE, glm::value_ptr(projView));

        glUseProgram(0);
    }


    m_FBO.get()->deleteRenderBuffer();
    m_FBO.get()->deleteDepthTexture();
    m_FBO.get()->deleteColorTexture();
    m_FBO.get()->deleteFrameBuffer();
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_FBO.get()->setFboWidth(m_screen_width);
    m_FBO.get()->setFboHeight(m_screen_height);
    m_FBO.get()->makeFBO();

    glUseProgram(m_volumeShader);  // Pass camera mat (proj * view)
    glUniform1f(glGetUniformLocation(m_volumeShader , "xMax"), m_camera.xMax());
    glUseProgram(0);
}

void Realtime::volumeChanged() {
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (!glInitialized) return;  // avoid gl calls before initialization finishes

    makeCurrent();

    glUseProgram(m_terrainShader);
    // Light
    glUniform1f(glGetUniformLocation(m_terrainShader , "testLight.longitude"), settings.lightData.longitude);
    glUniform1f(glGetUniformLocation(m_terrainShader , "testLight.latitude"), settings.lightData.latitude);
    glUniform1i(glGetUniformLocation(m_terrainShader , "testLight.type"), settings.lightData.type);
    glUniform3fv(glGetUniformLocation(m_terrainShader , "testLight.dir"), 1, glm::value_ptr(settings.lightData.dir));
    glUniform3fv(glGetUniformLocation(m_terrainShader , "testLight.color"), 1, glm::value_ptr(settings.lightData.color));
    glUniform4fv(glGetUniformLocation(m_terrainShader , "testLight.pos"), 1, glm::value_ptr(settings.lightData.pos));


    glUseProgram(m_volumeShader);

    // Volume
    glUniform3fv(glGetUniformLocation(m_volumeShader, "volumeScaling"), 1, glm::value_ptr(settings.volumeScaling));
    glUniform3fv(glGetUniformLocation(m_volumeShader, "volumeTranslate"), 1, glm::value_ptr(settings.volumeTranslate));
    glUniform1i(glGetUniformLocation(m_volumeShader, "numSteps"), settings.numSteps);
//        glUniform1f(glGetUniformLocation(m_volumeShader, "stepSize"), settings.stepSize);

    // Render Params
    glUniform1f(glGetUniformLocation(m_volumeShader, "densityMult"), settings.densityMult);
    glUniform1i(glGetUniformLocation(m_volumeShader, "invertDensity"), settings.invertDensity);
    glUniform1i(glGetUniformLocation(m_volumeShader, "gammaCorrect"), settings.gammaCorrect);
    glUniform1f(glGetUniformLocation(m_volumeShader, "cloudLightAbsorptionMult"), settings.cloudLightAbsorptionMult);
    glUniform1f(glGetUniformLocation(m_volumeShader, "minLightTransmittance"), settings.minLightTransmittance);

    // Shape texture: hi-res
    glUniform4fv(glGetUniformLocation(m_volumeShader , "hiResNoiseScaling"), 1, glm::value_ptr(settings.hiResNoise.scaling));
    glUniform3fv(glGetUniformLocation(m_volumeShader, "hiResNoiseTranslate"), 1, glm::value_ptr(settings.hiResNoise.translate));
    glUniform4fv(glGetUniformLocation(m_volumeShader, "hiResChannelWeights"), 1, glm::value_ptr(settings.hiResNoise.channelWeights));
    glUniform1f(glGetUniformLocation(m_volumeShader , "hiResDensityOffset"), settings.hiResNoise.densityOffset);

    // Detailed texture: low-res
    glUniform1f(glGetUniformLocation(m_volumeShader , "loResNoiseScaling"), settings.loResNoise.scaling[0]);
    glUniform3fv(glGetUniformLocation(m_volumeShader, "loResNoiseTranslate"), 1, glm::value_ptr(settings.loResNoise.translate));
    glUniform4fv(glGetUniformLocation(m_volumeShader, "loResChannelWeights"), 1, glm::value_ptr(settings.loResNoise.channelWeights));
    glUniform1f(glGetUniformLocation(m_volumeShader , "loResDensityWeight"), settings.loResNoise.densityWeight);

    // Light
    glUniform1f(glGetUniformLocation(m_volumeShader , "testLight.longitude"), settings.lightData.longitude);
    glUniform1f(glGetUniformLocation(m_volumeShader , "testLight.latitude"), settings.lightData.latitude);
    glUniform1i(glGetUniformLocation(m_volumeShader , "testLight.type"), settings.lightData.type);
    glUniform3fv(glGetUniformLocation(m_volumeShader , "testLight.dir"), 1, glm::value_ptr(settings.lightData.dir));
    glUniform3fv(glGetUniformLocation(m_volumeShader , "testLight.color"), 1, glm::value_ptr(settings.lightData.color));
    glUniform4fv(glGetUniformLocation(m_volumeShader , "testLight.pos"), 1, glm::value_ptr(settings.lightData.pos));
    std::cout << glm::to_string(settings.lightData.dir) << glm::to_string(settings.lightData.color) << '\n';

    glUseProgram(m_worleyShader);
    auto newArray = settings.newFineArray || settings.newMediumArray || settings.newCoarseArray;
    if (newArray) {
        int texSlot = settings.curSlot;
        int channelIdx = settings.curChannel;
        std::cout << "check" << texSlot << " " << channelIdx << '\n';

        // pass uniforms
        const auto &noiseParams = texSlot == 0 ? settings.hiResNoise : settings.loResNoise;
        glUniform1f(glGetUniformLocation(m_worleyShader, "persistence"), noiseParams.persistence);
        glUniform1i(glGetUniformLocation(m_worleyShader, "volumeResolution"), noiseParams.resolution);

        const auto &volumeTex = texSlot == 0 ? volumeTexHighRes : volumeTexLowRes;
        glBindImageTexture(0, volumeTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);


        glm::vec4 channelMask(0.f);
        channelMask[channelIdx] = 1.f;
        glUniform4fv(glGetUniformLocation(m_worleyShader, "channelMask"), 1, glm::value_ptr(channelMask));

        const auto &worleyPointsParams = noiseParams.worleyPointsParams[channelIdx];
        std::cout << "ss" << worleyPointsParams.cellsPerAxisFine << '\n';
        updateWorleyPoints(worleyPointsParams);  // generate new worley points into SSBO
        glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisFine"), worleyPointsParams.cellsPerAxisFine);
        glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisMedium"), worleyPointsParams.cellsPerAxisMedium);
        glUniform1i(glGetUniformLocation(m_worleyShader, "cellsPerAxisCoarse"), worleyPointsParams.cellsPerAxisCoarse);
        glDispatchCompute(noiseParams.resolution, noiseParams.resolution, noiseParams.resolution);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
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
     m_prevMousePos = event->pos();
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}
//void Realtime::wheelEvent(QWheelEvent *event) {
//    m_zoom -= event->angleDelta().y() / 1000.f;
//    rebuildMatrices();
//}

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

        makeCurrent();
        glUseProgram(m_volumeShader);  // Pass camera mat (proj * view)
        glUniformMatrix4fv(glGetUniformLocation(m_volumeShader, "viewInverse"), 1, GL_FALSE, glm::value_ptr(m_camera.getViewMatrixInverse()));
        glUseProgram(0);

        glUseProgram(m_terrainShader);  // Pass camera mat (proj * view)
//        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glm::mat4 projView = m_camera.getProjMatrix() * m_camera.getViewMatrix() * m_world;
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projViewMatrix"), 1, GL_FALSE, glm::value_ptr(projView));

        glm::mat4 transInv = glm::transpose(glm::inverse(m_camera.getViewMatrix() * m_world));
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "transInvViewMatrix"), 1, GL_FALSE, glm::value_ptr(transInv));

        glUseProgram(0);

        m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
        m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
        m_prevMousePos = event->pos();

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
    m_camera.updateViewMatrix();  // also stores u, v, w in viewInverse
    makeCurrent();
    glUseProgram(m_volumeShader);  // Pass camera uniforms
    glUniform3fv(glGetUniformLocation(m_volumeShader, "rayOrigWorld"), 1, glm::value_ptr(m_camera.getPos()));
    glUseProgram(0);

    glUseProgram(m_terrainShader);  // Pass camera mat (proj * view)

    glm::mat4 projView = m_camera.getProjMatrix() * m_camera.getViewMatrix() * m_world;
    glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projViewMatrix"), 1, GL_FALSE, glm::value_ptr(projView));

    glm::mat4 transInv = glm::transpose(glm::inverse(m_camera.getViewMatrix() * m_world));
    glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "transInvViewMatrix"), 1, GL_FALSE, glm::value_ptr(transInv));

    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}


void Realtime::rebuildMatrices() {
    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(-10 * m_angleX,QVector3D(0,0,1));
    QVector3D eye = QVector3D(1,1,1);
    eye = rot.map(eye);
    rot.setToIdentity();
    rot.rotate(-10 * m_angleY,QVector3D::crossProduct(QVector3D(0,0,1),eye));
    eye = rot.map(eye);

    eye = eye * m_zoom;

   /* m_terrain_camera = glm::mat4(1);
    m_terrain_camera = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]),glm::vec3(0,0,0),glm::vec3(0,0,1));

    m_proj = glm::mat4(1.f);
    m_proj = glm::perspective(45.0f, (float)width() / height(), 0.01f, 100.f)*/;

    m_camera.setPos(glm::vec4(eye[0], eye[1], eye[2], 1.0));
    m_proj = m_camera.getProjMatrix();


    update();
}
