#include <QWidget>
#include <QSurfaceFormat>
#include <QVector>
#include <QVector3D>
#include "Newpson/ObjParser/objparser.h"
#include "Newpson/Mesh/mesh.h"
#include "Newpson/Viewer/newpsonviewerglwidget.h"

namespace Newpson::Ui {

GLWidget::GLWidget(QWidget *parent):
    QOpenGLWidget(parent)
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setVersion(2, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);
    Newpson::ObjParser::ParserResult result;
    m_mesh = Newpson::ObjParser::load("/home/newpson/temp/bugatti.obj", result);
    if (result.status != Newpson::ObjParser::STATUS_OK)
        qDebug() << "Loading mesh error:" << Newpson::ObjParser::statusToString(result.status);

    Newpson::Mesh triMesh = m_mesh.triangulate();
    const QVector<int> &facesEnds = triMesh.facesEnds();
    const QVector<QVector3D> &vertices = triMesh.vertices();
    const QVector<QVector3D> &normals = triMesh.normals();
    const QVector<int> &indicesVertices = triMesh.indicesVertices();
    const QVector<int> &indicesNormals = triMesh.indicesNormals();

    m_rawData.reserve(facesEnds.length() * 6 * 3);
    int faceBegin = 0;
    for (int faceEnd : facesEnds) {
        for (int i = faceBegin; i < faceEnd; ++i) {
            m_rawData.append(vertices[indicesVertices[i]].x());
            m_rawData.append(vertices[indicesVertices[i]].y());
            m_rawData.append(vertices[indicesVertices[i]].z());
            m_rawData.append(normals[indicesNormals[i]].x());
            m_rawData.append(normals[indicesNormals[i]].y());
            m_rawData.append(normals[indicesNormals[i]].z());
        }
        faceBegin = faceEnd;
    }

    qDebug() << "Buffer size:" << m_rawData.length() * sizeof(float) / 1024 / 1024 << "MB";
    qDebug() << "Number of triangles:" << facesEnds.length();
    qDebug() << "Done.";
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

//     const float data[] = {
// //      vertices        normals
//         0.0, 0.0, 0.0,  0.0, 0.0, 1.0,
//         1.0, 0.0, 0.0,  0.0, 1.0, 1.0,
//         0.0, 1.0, 0.0,  1.0, 0.0, 1.0
//     };

    const float *data = m_rawData.data();

    m_objVbo.allocate(data, m_rawData.length() * sizeof(float));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    m_objVbo.release();

    m_projection.setToIdentity();
    m_view.lookAt(QVector3D(8, 4, 8), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    m_program.setUniformValue(m_viewLoc, m_view);

    m_program.release();

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_program.bind();

    glDrawArrays(GL_TRIANGLES, 0, m_rawData.length() / 6);
}

void GLWidget::resizeGL(int width, int height)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, (float) width / height, 0.01f, 1000.0f);
    m_program.bind();
    m_program.setUniformValue(m_projectionLoc, m_projection);
}

} // namespace Newpson::Ui
