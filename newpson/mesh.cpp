#include "mesh.hpp"
#include <QVector3D>

namespace Newpson  {

Mesh::Mesh(
    const QVector<QVector3D> &geometry,
    const QVector<QVector2D> &textures,
    const QVector<QVector3D> &normals,
    const QVector<int> &indicesGeometry,
    const QVector<int> &indicesTextures,
    const QVector<int> &indicesNormals,
    const QVector<int> &facesGeometry,
    const QVector<int> &facesTextures,
    const QVector<int> &facesNormals,
    const QVector<int> &polyGroups):

    m_vertices(geometry),
    m_verticesTexture(textures),
    m_normals(normals),
    m_indicesVertices(indicesGeometry),
    m_indicesVerticesTexture(indicesTextures),
    m_indicesNormals(indicesNormals),
    m_facesVertices(facesGeometry),
    m_facesVerticesTexture(facesTextures),
    m_facesNormals(facesNormals),
    m_polyGroups(polyGroups)
{}

// make verbose name + is...
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

const QVector<int> &Mesh::polyGroups() const
{
    return m_polyGroups;
}

} // namespace Newpson
