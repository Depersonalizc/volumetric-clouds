#pragma once
#include <vector>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

class FBO
{
public:
    FBO(int default_fbo, int fbo_width, int fbo_height);
    void init();
    void bind();
    void unbind();
    void send(int buffer_size, const void * data_ptr);
    void update_data(int new_size, const void * new_data_ptr);
    void paintTexture(GLuint texture, bool post_processing, int post_processing_type);

    // Get
    int getFBOWidth();
    int getFBOHeight();
    GLuint getFullscreenVbo();
    GLuint getFullscreenVao();
    GLuint getFbo();
//    GLuint getDefaultFbo();
    GLuint getFboDepthTexture();
    GLuint getFboColorTexture();

    // Delete
    void deleteDepthTexture();
    void deleteColorTexture();
    void deleteRenderBuffer();
    void deleteFrameBuffer();

    // Set
    void setFboWidth(int new_fbo_width);
    void setFboHeight(int new_fbo_height);

    // Functionality
    void generateBindFullscreen();
    void makeFBO();
    void paintTexture(bool post_processing, int post_processing_type);



private:
    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint m_fbo;
    GLuint m_fbo_depth_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fbo_color_texture;
    std::vector<GLuint> m_DrawBuffers = std::vector<GLuint>(3);




    std::vector<GLfloat> fullscreen_quad_data =
    {   //   POSITIONS    //  UV   //
        -1.0f,  1.0f, 0.0f, 0.f,  1.f,
        -1.0f, -1.0f, 0.0f, 0.f,  0.f,
         1.0f, -1.0f, 0.0f, 1.f,  0.f,

         1.0f,  1.0f, 0.0f, 1.f,  1.f,
        -1.0f,  1.0f, 0.0f, 0.f,  1.f,
         1.0f, -1.0f, 0.0f, 1.f,  0.f,
    };


};
