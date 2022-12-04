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


    // Terrain VAO/VBO
//    std::vector<GLfloat> verts = m_terrain.generateTerrain();
//    std::cout<<verts[30]<<std::endl;

//    glGenBuffers(1, &m_terrainVbo);
//    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
//    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
//    glGenVertexArrays(1, &m_terrainVao);
//    glBindVertexArray(m_terrainVao);
//    glEnableVertexAttribArray(0);  // position
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);

//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
//                             nullptr);

//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
//                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));

//    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
//                             reinterpret_cast<void *>(6 * sizeof(GLfloat)));

//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

//    m_world.setToIdentity();
//    m_world.translate(QVector3D(-0.5,-0.5,0));

//    m_camera_terrain.setToIdentity();
//    m_camera_terrain.lookAt(QVector3D(1,1,1),QVector3D(0,0,0),QVector3D(0,0,1));

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

    // FBO related
    m_defaultFBO = 2;
    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;


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
    m_terrain_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain_texture.vert", ":/resources/shaders/terrain_texture.frag");

    //m_terrainShader = ShaderLoader::createShaderProgram(":/resources/shaders/vertex.vert", ":/resources/shaders/fragment.frag");


    /* Set up VBO, VAO, and SSBO */
    setUpVolume();

    /* Set up default camera */
    m_camera = Camera(SceneCameraData(), size().width(), size().height(), settings.nearPlane, settings.farPlane);

//    // Pass uniforms to the terrain shader
//    glUseProgram(m_terrainShader);
//    {
//        glUniform1i(glGetUniformLocation(m_terrainShader, "wireshade"), m_terrain.m_wireshade);
//        //QMatrix4x4 mv = m_camera_terrain * m_world;
//        //glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projMatrix"), 1, GL_FALSE, (m_proj.data()));
//        //glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "mvMatrix"), 1, GL_FALSE, (mv.data()));

//        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));

//        glm::mat4 mview;
//        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "mvMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera.getViewMatrix()));

//    }


    // Terrain shader related
    {
        glClearColor(0, 0, 0, 1);
        m_program = new QOpenGLShaderProgram;
        std::cout << QDir::currentPath().toStdString() << std::endl;
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/resources/shaders/terrain_generator.vert");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/resources/shaders/terrain_generator.frag");
        m_program->link();
        m_program->bind();

        //m_terrainShader = ShaderLoader::createShaderProgram(":/resources/shaders/vertex.vert", ":/resources/shaders/fragment.frag");

        m_projMatrixLoc = m_program->uniformLocation("projMatrix");
        m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
        m_terrainVao.create();
        m_terrainVao.bind();

        std::vector<GLfloat> verts = m_terrain.generateTerrain();
        std::cout<<verts[30]<<std::endl;

        m_terrainVbo.create();
        m_terrainVbo.bind();
        m_terrainVbo.allocate(verts.data(),verts.size()*sizeof(GLfloat));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));

        m_terrainVbo.release();
        m_world.setToIdentity();
        m_world.translate(QVector3D(-0.5,-0.5,0));

        m_camera_terrain.setToIdentity();
        m_camera_terrain.lookAt(QVector3D(1,1,1),QVector3D(0,0,0),QVector3D(0,0,1));
        m_program->release();

    }



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
    prepTexture();
}

void Realtime::prepTexture() {

    glUseProgram(m_terrain_texture_shader);
    glUniform1i(glGetUniformLocation(m_terrain_texture_shader, "sampler_texture"), 0);
    glUseProgram(0);

    // "fullscreen" quad's vertex data
    std::vector<GLfloat> fullscreen_quad_data =
    { //     POSITIONS    //
        -1.f,  1.f, 0.0f,
         0.f,  1.f,
        -1.f, -1.f, 0.0f,
         0.f,  0.f,
         1.f,  1.f, 0.0f,
         1.f,  1.f,
         1.f,  1.f, 0.0f,
         1.f,  1.f,
        -1.f,  -1.f, 0.0f,
         0.f,  0.f,
         1.f, -1.f, 0.0f,
         1.f,  0.f
    };
    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
    glUseProgram(0);
}

void Realtime::makeFBO(){
    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind --- this is the texture for colors
    glGenTextures(1, &m_fbo_texture_color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture_color);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind --- this is the texture for depths
    glGenTextures(1, &m_fbo_texture_depth);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture_depth);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Add our texture_color as a color attachment, and our texture_depth as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture_color, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_fbo_texture_depth, 0);

    std::cout << m_fbo_texture_color << std::endl;
    std::cout << m_fbo_texture_depth << std::endl;

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

}

void Realtime::paintGL() {
    // Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Call glViewport
    glViewport(0,0,m_fbo_width, m_fbo_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    Debug::checkOpenGLErrors();

    //drawVolume();

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawTerrain();

    // After drawing the scene: Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0,0,m_fbo_width, m_fbo_height);

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Call paintTexture to draw our FBO color attachment texture
    paintTexture(m_fbo_texture_color);
}

void Realtime::drawTerrain() {
    glEnable(GL_DEPTH_TEST);
    //std::cout<<"here"<<std::endl;

    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera_terrain * m_world);
    m_program->setUniformValue(m_program->uniformLocation("wireshade"),m_terrain.m_wireshade);

    int res = m_terrain.getResolution();

    glPolygonMode(GL_FRONT_AND_BACK,m_terrain.m_wireshade? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);

    m_program->release();
}

void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_terrain_texture_shader);

    // Pass in uniforms
    glUniform1f(glGetUniformLocation(m_terrain_texture_shader, "w"), size().width());
    glUniform1f(glGetUniformLocation(m_terrain_texture_shader, "h"), size().height());

    glBindVertexArray(m_fullscreen_vao);
    // Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
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

    // Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture_color);
    glDeleteTextures(1, &m_fbo_texture_depth);
    glDeleteFramebuffers(1, &m_fbo);

    m_fbo_width = w * m_devicePixelRatio;
    m_fbo_height = h * m_devicePixelRatio;
    // Regenerate your FBO
    makeFBO();

    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
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
     m_prevMousePos = event->pos();
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}
void Realtime::wheelEvent(QWheelEvent *event) {
    m_zoom -= event->angleDelta().y() / 100.f;
    rebuildMatrices();
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

        glUseProgram(m_terrainShader);  // Pass camera mat (proj * view)
        glUniformMatrix4fv(glGetUniformLocation(m_terrainShader, "projView"), 1, GL_FALSE, glm::value_ptr(m_camera.getProjView()));
        glUseProgram(0);

        m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
        m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
        m_prevMousePos = event->pos();
        rebuildMatrices();



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

void Realtime::rebuildMatrices() {
    m_camera_terrain.setToIdentity();
    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(-10 * m_angleX,QVector3D(0,0,1));
    QVector3D eye = QVector3D(1,1,1);
    eye = rot.map(eye);
    rot.setToIdentity();
    rot.rotate(-10 * m_angleY,QVector3D::crossProduct(QVector3D(0,0,1),eye));
    eye = rot.map(eye);

    eye = eye * m_zoom;

    m_camera_terrain.lookAt(eye,QVector3D(0,0,0),QVector3D(0,0,1));

    m_proj.setToIdentity();
    m_proj.perspective(45.0f, 1.0 * width() / height(), 0.01f, 100.0f);

    update();
}
