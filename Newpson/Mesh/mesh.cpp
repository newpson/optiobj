#include <QVector3D>
#include <QDebug>
#include "Newpson/Mesh/mesh.h"

namespace Newpson  {

Mesh::Mesh(
    const QVector<QVector3D> &vertices,
    const QVector<QVector2D> &verticesTexture,
    const QVector<QVector3D> &normals,
    const QVector<int> &indicesVertices,
    const QVector<int> &indicesVerticesTexture,
    const QVector<int> &indicesNormals,
    const QVector<int> &facesEnds):

    m_vertices(vertices),
    m_verticesTexture(verticesTexture),
    m_normals(normals),
    m_indicesVertices(indicesVertices),
    m_indicesVerticesTexture(indicesVerticesTexture),
    m_indicesNormals(indicesNormals),
    m_facesEnds(facesEnds)
{
    const int numIndicesVertices = facesEnds.length();
    m_groupsEnds = {numIndicesVertices};
}

Mesh::Mesh(
    const QVector<QVector3D> &vertices,
    const QVector<QVector2D> &verticesTexture,
    const QVector<QVector3D> &normals,
    const QVector<int> &indicesVertices,
    const QVector<int> &indicesVerticesTexture,
    const QVector<int> &indicesNormals,
    const QVector<int> &facesEnds,
    const QVector<QString> &groupsNames,
    const QVector<int> &groupsEnds):

    m_vertices(vertices),
    m_verticesTexture(verticesTexture),
    m_normals(normals),
    m_indicesVertices(indicesVertices),
    m_indicesVerticesTexture(indicesVerticesTexture),
    m_indicesNormals(indicesNormals),
    m_facesEnds(facesEnds),
    m_groupsNames(groupsNames),
    m_groupsEnds(groupsEnds)
{}

bool areIndicesValid(const QVector<int> &indices, const int maxIndex)
{
    for (int index : indices) {
        const bool outOfBounds = index < 0 || index >= maxIndex;
        if (outOfBounds)
            return false;
    }
    return true;
}

bool areFaceEndsValid(const QVector<int> &indices, const int maxIndex)
{
    int lastIndex = -1;
    for (int index : indices) {
        const bool isOutOfBounds = index < 0 || index > maxIndex;
        const bool isMissordered = index < lastIndex;
        if (isOutOfBounds || isMissordered)
            return false;
        lastIndex = index;
    }
    return true;
}

Mesh::ValidationResult Mesh::checkConsistency() const
{
    if (!areIndicesValid(m_indicesVertices, m_vertices.length()))
        return VALIDATION_ERROR_INVALID_INDICES_VERTICES;

    if (!areIndicesValid(m_indicesVerticesTexture, m_verticesTexture.length()))
        return VALIDATION_ERROR_INVALID_INDICES_VERTICES_TEXTURE;

    if (!areIndicesValid(m_indicesNormals, m_normals.length()))
        return VALIDATION_ERROR_INVALID_INDICES_NORMALS;

    if (!areFaceEndsValid(m_facesEnds, m_indicesVertices.length()))
        return VALIDATION_ERROR_INVALID_FACES_VERTICES;

    const bool hasNoGroups = m_groupsEnds.isEmpty() || m_groupsNames.isEmpty();
    if (hasNoGroups)
        return VALIDATION_ERROR_NO_GROUPS;

    const bool hasNoDefaultGroup = m_groupsNames[0] != "default";
    if (hasNoDefaultGroup)
        return VALIDATION_ERROR_NO_GROUP_DEFAULT;

    // FIXME rename more general
    if (!areFaceEndsValid(m_groupsEnds, m_facesEnds.length()))
        return VALIDATION_ERROR_INVALID_INDICES_GROUPS;

    return VALIDATION_OK;
}

//Mesh Mesh::triangulate() const
//{
//    QVector<int> indicesVertices;
//    QVector<int> indicesVerticesTexture;
//    QVector<int> indicesNormals;
//    QVector<int> facesEnds;
//    QVector<int> facesEndsTexture;
//    QVector<int> facesNormals;
//    QVector<int> groupsEnds;

//    int groupBegin = 0;
//    int faceBegin = 0;
//    for (int groupEnd : m_groupsEnds)
//    {
//        qDebug() << "{";
//        int faceEnd = -1;
//        for (int i = groupBegin; i < groupEnd; ++i) {
//            qDebug() << i << "has normals:" << faceHasComponent(m_facesEnds, i);
////            faceEnd = m_facesEnds[i];
////            qDebug() << "  {";
////            for (int j = faceBegin; j < faceEnd; ++j) {
////                    qDebug() << "    " << m_indicesVertices[j];
////            }
////            qDebug() << "  }";
////            faceBegin = faceEnd;
//        }
//        qDebug() << "}";
//        groupBegin = groupEnd;
//    }

//    return Mesh();
////    return Mesh(m_vertices,
////                m_verticesTexture,
////                m_normals,
////                indicesVertices,
////                indicesVerticesTexture,
////                indicesNormals,
////                facesEnds,
////                facesEndsTexture,
////                facesNormals,
////                m_groupsNames,
////                groupsEnds);
//}

const QVector<QVector3D> &Mesh::vertices() const
{
    return m_vertices;
}

const QVector<QVector2D> &Mesh::verticesTexture() const
{
    return m_verticesTexture;
}

const QVector<QVector3D> &Mesh::normals() const
{
    return m_normals;
}

const QVector<int> &Mesh::indicesVertices() const
{
    return m_indicesVertices;
}

const QVector<int> &Mesh::indicesVerticesTexture() const
{
    return m_indicesVerticesTexture;
}

const QVector<int> &Mesh::indicesNormals() const
{
    return m_indicesNormals;
}

const QVector<int> &Mesh::facesEnds() const
{
    return m_facesEnds;
}

const QVector<QString> &Mesh::groupsNames() const
{
    return m_groupsNames;
}

const QVector<int> &Mesh::groupsEnds() const
{
    return m_groupsEnds;
}

}
