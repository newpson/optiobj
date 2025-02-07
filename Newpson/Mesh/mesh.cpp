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
    const QVector<int> &facesVertices,
    const QVector<int> &facesVerticesTexture,
    const QVector<int> &facesNormals):

    m_vertices(vertices),
    m_verticesTexture(verticesTexture),
    m_normals(normals),
    m_indicesVertices(indicesVertices),
    m_indicesVerticesTexture(indicesVerticesTexture),
    m_indicesNormals(indicesNormals),
    m_facesVertices(facesVertices),
    m_facesVerticesTexture(facesVerticesTexture),
    m_facesNormals(facesNormals),
    m_groupsEnds({facesVertices.length()})
{}

Mesh::Mesh(
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
    const QVector<int> &groupsEnds):

    m_vertices(vertices),
    m_verticesTexture(verticesTexture),
    m_normals(normals),
    m_indicesVertices(indicesVertices),
    m_indicesVerticesTexture(indicesVerticesTexture),
    m_indicesNormals(indicesNormals),
    m_facesVertices(facesVertices),
    m_facesVerticesTexture(facesVerticesTexture),
    m_facesNormals(facesNormals),
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

bool areFaceIndicesValid(const QVector<int> &indices, const int maxIndex)
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
    bool areFacesCoherent = (m_facesVertices.length() == m_facesVerticesTexture.length()
                             && m_facesVertices.length() == m_facesNormals.length());
    if (!areFacesCoherent) {
        return VALIDATION_ERROR_FACES_INCOHERENT;
    }

    if (!areIndicesValid(m_indicesVertices, m_vertices.length()))
        return VALIDATION_ERROR_INVALID_INDICES_VERTICES;

    if (!areIndicesValid(m_indicesVerticesTexture, m_verticesTexture.length()))
        return VALIDATION_ERROR_INVALID_INDICES_VERTICES_TEXTURE;

    if (!areIndicesValid(m_indicesNormals, m_normals.length()))
        return VALIDATION_ERROR_INVALID_INDICES_NORMALS;

    if (!areFaceIndicesValid(m_facesVertices, m_indicesVertices.length()))
        return VALIDATION_ERROR_INVALID_FACES_VERTICES;

    if (!areFaceIndicesValid(m_facesVerticesTexture, m_indicesVerticesTexture.length()))
        return VALIDATION_ERROR_INVALID_FACES_VERTICES_TEXTURE;

    if (!areFaceIndicesValid(m_facesNormals, m_indicesNormals.length()))
        return VALIDATION_ERROR_INVALID_FACES_NORMALS;

    const bool hasNoGroups = m_groupsEnds.isEmpty() || m_groupsNames.isEmpty();
    if (hasNoGroups)
        return VALIDATION_ERROR_NO_GROUPS;

    const bool hasNoDefaultGroup = m_groupsNames[0] != "default";
    if (hasNoDefaultGroup)
        return VALIDATION_ERROR_NO_GROUP_DEFAULT;

    if (!areFaceIndicesValid(m_groupsEnds, m_facesVertices.length()))
        return VALIDATION_ERROR_INVALID_INDICES_GROUPS;

    return VALIDATION_OK;
}

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

const QVector<int> &Mesh::facesVertices() const
{
    return m_facesVertices;
}

const QVector<int> &Mesh::facesVerticesTexture() const
{
    return m_facesVerticesTexture;
}

const QVector<int> &Mesh::facesNormals() const
{
    return m_facesNormals;
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
