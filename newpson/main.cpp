#include "Mesh.hpp"

#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>

int main()
{
    Newpson::Mesh mesh;
    auto status = Newpson::Mesh::read(mesh, PROJECT_ASSETS "/bad.obj");

    if (status != Newpson::Mesh::STATUS_OK) {
        qDebug() << Newpson::Mesh::statusToString(status);
        qDebug() << "Aborting.";
        return -1;
    }

    return 0;
}
