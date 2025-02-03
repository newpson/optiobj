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
    m_facesGeometry(facesGeometry),
    m_facesTextures(facesTextures),
    m_facesNormals(facesNormals),
    m_polyGroups(polyGroups)
{}

// make verbose name + is...
bool Mesh::validate() const
{
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
    return m_facesGeometry;
}

const QVector<int> &Mesh::facesVerticesTexture() const
{
    return m_facesTextures;
}

const QVector<int> &Mesh::facesNormals() const
{
    return m_facesNormals;
}

const QVector<int> &Mesh::polyGroups() const
{
    return m_facesTextures;
}

} // namespace Newpson
