#ifndef NEWPSON_VIEWER_GLWIDGET_H
#define NEWPSON_VIEWER_GLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector>
#include "Newpson/Mesh/mesh.h"
#include "Newpson/Viewer/camera.h"

namespace Newpson::Ui {

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum MovementType
    {
        MOVEMENT_NONE,
        MOVEMENT_ROTATION,
        MOVEMENT_SLIDE,
    };
    GLWidget(QWidget *parent = nullptr);

protected:
    // QOpenGLWidget
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    // QWidget
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Newpson::Mesh m_mesh;
    QOpenGLBuffer m_objVbo;
    QVector<float> m_rawData;

    QOpenGLShaderProgram m_program;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    int m_projectionLoc;
    int m_viewLoc;
    int m_dirLoc;
    QPointF m_mousePositionLast;

    Camera m_camera;

    MovementType m_movementType = MOVEMENT_NONE;
};

}

#endif
