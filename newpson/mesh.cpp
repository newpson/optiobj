#include "mesh.hpp"
#include <QVector3D>

namespace Newpson  {

Mesh::Mesh():
    m_groupsBegins({0}),
    m_groupsEnds({1}),
    m_groupsNames({"default"})
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
    m_groupsBegins({0}),
    m_groupsEnds({1}),
    m_groupsNames({"default"})
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
    const QVector<int> &groupsBegins,
    const QVector<int> &groupsEnds,
    const QVector<QString> &groupsNames):

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
    m_groupsBegins(groupsBegins),
    m_groupsEnds(groupsEnds)
{}

bool Mesh::isValid() const
{
    if (!(m_facesVertices.length() == m_facesVerticesTexture.length()
            && m_facesVertices.length() == m_facesNormals.length()))
        return false;

    for (int index : m_indicesVertices)
        if (index < 0 || index >= m_vertices.length())
            return false;

    for (int index : m_indicesVerticesTexture)
        if (index < 0 || index >= m_verticesTexture.length())
            return false;

    for (int index : m_indicesNormals)
        if (index < 0 || index >= m_normals.length())
            return false;

    for (int index : m_facesVertices)
        if (index < 0 || index > m_indicesVertices.length())
            return false;

    for (int index : m_facesVerticesTexture)
        if (index < 0 || index > m_indicesVerticesTexture.length())
            return false;

    for (int index : m_facesNormals)
        if (index < 0 || index > m_indicesNormals.length())
            return false;

    const bool hasNoGroups = m_groupsBegins.isEmpty() || m_groupsEnds.isEmpty() || m_groupsNames.isEmpty();
    if (hasNoGroups)
        return false;

    const bool hasNoDefaultGroup = m_groupsBegins[0] != 0 || m_groupsNames[0] != "default";
    if (hasNoDefaultGroup)
        return false;

    return true;
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

const QVector<int> &Mesh::groupsBegins() const
{
    return m_groupsBegins;
}

const QVector<int> &Mesh::groupsEnds() const
{
    return m_groupsEnds;
}

} // namespace Newpson
