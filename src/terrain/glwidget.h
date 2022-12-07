#pragma once

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include "terraingenerator.h"
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget
{
public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private:
    void rebuildMatrices();

    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLVertexArrayObject m_terrainVao;
    QOpenGLBuffer m_terrainVbo;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    TerrainGenerator m_terrain;
    int m_projMatrixLoc = 0;
    int m_mvMatrixLoc = 0;

    QPoint m_prevMousePos;
    float m_angleX;
    float m_angleY;
    float m_zoom;

};
