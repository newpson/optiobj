#ifndef NEWPSON_VIEWER_GLWIDGET_H
#define NEWPSON_VIEWER_GLWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector>

#include "Newpson/Mesh/mesh.h"
#include "Newpson/Viewer/Camera/camera.h"
#include "Newpson/Viewer/GLStorage/glstorage.h"

namespace Newpson::Viewer {

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
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram m_program;

    // to be moved to storage
    Newpson::Mesh m_mesh;
    QVector<float> m_rawData;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    int m_projectionLoc;
    int m_viewLoc;
    int m_dirLoc;
    //

    Camera m_camera;
    MovementType m_movementType = MOVEMENT_NONE;
    QPointF m_mousePositionLast = {0.0, 0.0};
};

}

#endif
