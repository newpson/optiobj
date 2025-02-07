#ifndef NEWPSON_MESH_H
#define NEWPSON_MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson {

class Mesh
{
public:
    enum ValidationResult {
        VALIDATION_OK,

        VALIDATION_ERROR,
        VALIDATION_ERROR_BEGIN,
        VALIDATION_ERROR_FACES_INCOHERENT,
        VALIDATION_ERROR_INVALID_INDICES_VERTICES,
        VALIDATION_ERROR_INVALID_INDICES_VERTICES_TEXTURE,
        VALIDATION_ERROR_INVALID_INDICES_NORMALS,
        VALIDATION_ERROR_INVALID_INDICES_GROUPS,
        VALIDATION_ERROR_INVALID_FACES_VERTICES,
        VALIDATION_ERROR_INVALID_FACES_VERTICES_TEXTURE,
        VALIDATION_ERROR_INVALID_FACES_NORMALS,
        VALIDATION_ERROR_NO_GROUPS,
        VALIDATION_ERROR_NO_GROUP_DEFAULT,
        VALIDATION_ERROR_END,
    };

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
        const QVector<int> &facesNormals);

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
        const QVector<QString> &groupsNames,
        const QVector<int> &groupsEnds);

    const QVector<QVector3D> &vertices() const;
    const QVector<QVector2D> &verticesTexture() const;
    const QVector<QVector3D> &normals() const;
    const QVector<int> &indicesVertices() const;
    const QVector<int> &indicesVerticesTexture() const;
    const QVector<int> &indicesNormals() const;
    const QVector<int> &facesVertices() const;
    const QVector<int> &facesVerticesTexture() const;
    const QVector<int> &facesNormals() const;
    const QVector<QString> &groupsNames() const;
    const QVector<int> &groupsEnds() const;

    ValidationResult checkConsistency() const;
    Mesh triangulate() const;

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
    QVector<QString> m_groupsNames = {"default"};
    QVector<int> m_groupsEnds = {0};
};

}

#endif // NEWPSON_MESH_H
