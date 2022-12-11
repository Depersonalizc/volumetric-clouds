#include "FBO.h"
#include <iostream>
#include <ostream>
#include <iostream>

FBO::FBO(int default_fbo, int fbo_width, int fbo_height) {
    m_defaultFBO = default_fbo;
    m_fbo_width = fbo_width;
    m_fbo_height = fbo_height;
    init();
}

// GET functions
int FBO::getFBOWidth() {
    return m_fbo_width;
}

int FBO::getFBOHeight() {
    return m_fbo_height;
}

GLuint FBO::getFullscreenVbo() {
    return m_fullscreen_vbo;
}

GLuint FBO::getFullscreenVao() {
    return m_fullscreen_vao;
}

GLuint FBO::getFbo() {
    return m_fbo;
}

//GLuint FBO::getDefaultFbo() {
//    return m_defaultFBO;
//}

GLuint FBO::getFboDepthTexture() {
    return m_fbo_depth_texture;
}

GLuint FBO::getFboColorTexture() {
    return m_fbo_color_texture;
}

// DELETE
void FBO::deleteDepthTexture() {
    glDeleteTextures(1, &m_fbo_depth_texture);
}

void FBO::deleteColorTexture() {
    glDeleteTextures(1, &m_fbo_color_texture);
}

void FBO::deleteRenderBuffer() {
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
}

void FBO::deleteFrameBuffer() {
    glDeleteFramebuffers(1, &m_fbo);
}



// SET functions

void FBO::setFboWidth(int new_fbo_width) {
    m_fbo_width = new_fbo_width;
}

void FBO::setFboHeight(int new_fbo_height) {
    m_fbo_height = new_fbo_height;
}


// Functionality
void FBO::generateBindFullscreen() {
    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2, GL_FLOAT,GL_FALSE,5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FBO::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_depth_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_fbo_width, m_fbo_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

//    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
//    glGenRenderbuffers(1, &m_fbo_renderbuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind another texture for the color attachment ------ texture location 1
    glGenTextures(1, &m_fbo_color_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fbo_color_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_color_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    m_DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &m_DrawBuffers[0]);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
    }

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

}

//void FBO::makeFBO() {
//    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
//    glGenTextures(1, &m_fbo_texture);
//    glActiveTexture(0);
//    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_fbo_width, m_fbo_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
//    glGenRenderbuffers(1, &m_fbo_renderbuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);

//    // Task 18: Generate and bind an FBO
//    glGenFramebuffers(1, &m_fbo);
//    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

//    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_texture, 0);
//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

//    // Task 22: Unbind the FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
//}

void FBO::init() {
    generateBindFullscreen();
}

