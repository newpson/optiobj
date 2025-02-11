#ifndef NEWPSON_VIEWER_GLSTORAGE_H
#define NEWPSON_VIEWER_GLSTORAGE_H

#include <QtCore/QObject>
#include <QVector>

#include "Newpson/Mesh/mesh.h"

namespace Newpson::Viewer {

class GLStorage: public QObject
{
    Q_OBJECT
public:
    GLStorage();

    void load(const QString &path);

protected:
    QOpenGLBuffer m_objVbo;
    QVector<Newpson::Mesh> m_triangulatedMeshes;
    QVector<float> m_rawData;

    QOpenGLShaderProgram m_program;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    int m_projectionLoc;
    int m_viewLoc;
    int m_dirLoc;
    QPointF m_mousePositionLast;

};

}

#endif
