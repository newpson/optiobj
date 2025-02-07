#ifndef NEWPSON_UI_GLWIDGET_H
#define NEWPSON_UI_GLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector>
#include "mesh.h"

namespace Newpson::Ui {

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    Newpson::Mesh m_mesh;
    QOpenGLBuffer m_objVbo;
    QVector<float> m_rawData;

    QOpenGLShaderProgram m_program;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    int m_projectionLoc;
    int m_viewLoc;
};

} // namespace Newpson::Ui

#endif
