#ifndef NEWPSON_MESH_H
#define NEWPSON_MESH_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>

namespace Newpson {

class Mesh
{
public:
    Mesh() = default;

    QVector<QVector3D> geometryVertices;
    QVector<QVector<int>> geometryFaces;
    QVector<QVector2D> textureVertices;
    QVector<QVector<int>> textureFaces;
};

}

#endif // NEWPSON_MESH_H
