#ifndef NEWPSON_MESH_H
#define NEWPSON_MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson {

// should be immutable
class Mesh
{
public:
    Mesh() = default;
    Mesh(
        const QVector<QVector3D> &vertices,
        const QVector<QVector2D> &verticesTexture,
        const QVector<QVector3D> &normals,
        const QVector<int> &indicesVertices,
        const QVector<int> &indicesVerticesTexture,
        const QVector<int> &indicesNormals,
        const QVector<int> &facesVertices,
        const QVector<int> &facesVerticesTexture,
        const QVector<int> &facesNormals,
        const QVector<int> &polyGroups);

    const QVector<QVector3D> &vertices() const;
    const QVector<QVector2D> &verticesTexture() const;
    const QVector<QVector3D> &normals() const;
    const QVector<int> &indicesVertices() const;
    const QVector<int> &indicesVerticesTexture() const;
    const QVector<int> &indicesNormals() const;
    const QVector<int> &facesVertices() const;
    const QVector<int> &facesVerticesTexture() const;
    const QVector<int> &facesNormals() const;
    const QVector<int> &polyGroups() const;

    bool isValid() const;

private:
    QVector<QVector3D> m_vertices;
    QVector<QVector2D> m_verticesTexture;
    QVector<QVector3D> m_normals;
    QVector<int> m_indicesVertices;
    QVector<int> m_indicesVerticesTexture;
    QVector<int> m_indicesNormals;
    QVector<int> m_facesVertices;
    QVector<int> m_facesVerticesTexture;
    QVector<int> m_facesNormals;
    QVector<int> m_polyGroups;
};

}

#endif // NEWPSON_MESH_H
