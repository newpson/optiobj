#include "mesh.hpp"
#include <QVector3D>

namespace Newpson  {

void Mesh::clear()
{
    geometry.clear();
    textures.clear();
    normals.clear();

    indicesGeometry.clear();
    indicesTextures.clear();
    indicesNormals.clear();
    facesGeometry.clear();
    facesTextures.clear();
    facesNormals.clear();

    faceGroups.clear();
}

void Mesh::addGeometry(const QVector3D &vertex)
{
    geometry.append(vertex);
}

void Mesh::addTexture(const QVector2D &vertex)
{
    textures.append(vertex);
}

void Mesh::addNormal(const QVector3D &normal)
{
    normals.append(normal);
}

int Mesh::addFace(const bool hasTextures, const bool hasNormals)
{
    facesGeometry.append(indicesGeometry.length());
    facesTextures.append(hasTextures ? indicesTextures.length() : -1);
    facesNormals.append(hasNormals ? indicesNormals.length() : -1);
    return (facesGeometry.length() - 1);
}

void Mesh::addFaceComponentGeometry(const int faceIndex, const int indexGeometry)
{
    Q_ASSERT(faceIndex < facesGeometry.length());
    if (indexGeometry > 0)
        indicesGeometry.append(indexGeometry - 1);
    else if (indexGeometry < 0)
        indicesGeometry.append(normals.length() + indexGeometry);
}

void Mesh::addFaceComponentTexture(const int faceIndex, const int indexTexture)
{
    Q_ASSERT(faceIndex < facesTextures.length());
    if (indexTexture > 0)
        indicesTextures.append(indexTexture - 1);
    else if (indexTexture < 0)
        indicesTextures.append(normals.length() + indexTexture);
}

void Mesh::addFaceComponentNormal(const int faceIndex, const int indexNormal)
{
    Q_ASSERT(faceIndex < facesNormals.length());
    if (indexNormal > 0)
        indicesNormals.append(indexNormal - 1);
    else if (indexNormal < 0)
        indicesNormals.append(normals.length() + indexNormal);
}

int Mesh::numGeometry() const
{
    return geometry.length();
}

int Mesh::numTextures() const
{
    return textures.length();
}

int Mesh::numNormals() const
{
    return normals.length();
}

int Mesh::numFaces() const
{
    return facesGeometry.length();
}

} // namespace Newpson
