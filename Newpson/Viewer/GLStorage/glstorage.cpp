#include "Newpson/Viewer/GLStorage/glstorage.h"

#include "Newpson/Obj/Parser/parser.h"
#include "Newpson/Mesh/mesh.h"

namespace Newpson::Viewer {

GLStorage::GLStorage()
{
    qDebug() << "All ok.";
};

/*
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
*/

}
