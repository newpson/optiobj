#ifndef NEWPSON_MESH_H
#define NEWPSON_MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson {

class Mesh
{
public:
    Mesh() = default;

    void clear();

    void addGeometry(const QVector3D &vertex);
    void addTexture(const QVector2D &vertex);
    void addNormal(const QVector3D &normal);
    int addFace(const bool hasTextures, const bool hasNormals);
    void addFaceComponentGeometry(const int faceIndex, const int indexGeometry);
    void addFaceComponentTexture(const int faceIndex, const int indexTexture);
    void addFaceComponentNormal(const int faceIndex, const int indexNormal);
    int numGeometry() const;
    int numTextures() const;
    int numNormals() const;
    int numFaces() const;

    QVector<QVector3D> geometry;
    QVector<QVector2D> textures;
    QVector<QVector3D> normals;

    QVector<int> indicesGeometry;
    QVector<int> indicesTextures;
    QVector<int> indicesNormals;
    QVector<int> facesGeometry;
    QVector<int> facesTextures;
    QVector<int> facesNormals;

    QVector<int> faceGroups; // unimplemented
};

}

#endif // NEWPSON_MESH_H
