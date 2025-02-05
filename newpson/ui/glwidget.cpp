#include <QWidget>
#include <QSurfaceFormat>
#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent):
    QOpenGLWidget(parent)
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setVersion(2, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    qDebug() << fmt.version();
    setFormat(fmt);
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
        "#version 150\n"
        " \n"
        "in vec3 position; \n"
        "in vec3 normal; \n"
        "out vec3 frag_normal; \n"
        "uniform mat4 view; \n"
        "uniform mat4 projection; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    frag_normal = normal; \n"
        "    gl_Position = projection * view * vec4(position, 1.0); \n"
        "} \n")) {
        qDebug() << "Vertex shader compilation error:" << m_program.log();
        return;
    }
    if (!m_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
        "#version 150\n"
        " \n"
        "in highp vec3 frag_normal; \n"
        " \n"
        "void main() \n"
        "{ \n"
        "    gl_FragColor = vec4(frag_normal, 1.0); \n"
        "} \n")) {
        qDebug() << "Fragment shader compilation error:" << m_program.log();
        return;
    }

    m_program.bindAttributeLocation("position", 0);
    m_program.bindAttributeLocation("normal", 1);

    if (!m_program.link()) {
        qDebug() << "Program linkage error:" << m_program.log();
        return;
    }

    if (!m_program.bind())  {
        qDebug() << "Program binding error:" << m_program.log();
        return;
    }

    m_projectionLoc = m_program.uniformLocation("projection");
    m_viewLoc = m_program.uniformLocation("view");
    m_objVbo.create();
    m_objVbo.bind();

    const float data[] = {
//      vertices        normals
        0.0, 0.0, 0.0,  0.0, 0.0, 1.0,
        1.0, 0.0, 0.0,  0.0, 1.0, 1.0,
        0.0, 1.0, 0.0,  1.0, 0.0, 1.0
    };

    m_objVbo.allocate(data, sizeof(data));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    m_objVbo.release();

    m_view.setToIdentity();
    m_view.translate(-0.6, -0.6, -2);
    m_view.rotate(0, 1.0, 0, 0);

    m_program.setUniformValue(m_viewLoc, m_view);


    m_program.release();

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program.bind();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void GLWidget::resizeGL(int width, int height)
{
    m_program.bind();
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, (float) width / height, 0.01f, 100.0f);
    m_program.setUniformValue(m_projectionLoc, m_projection);
}
