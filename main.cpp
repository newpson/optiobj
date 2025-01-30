#include "Mesh.hpp"
#include "obj/Parser.hpp"

#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>

int main()
{
    Newpson::Mesh mesh;
    auto status = Newpson::Parsing::obj::Parser::load(PROJECT_ASSETS "/ok/cube.obj", mesh);

    if (status != Newpson::Parsing::obj::Parser::STATUS_OK) {
        qDebug() << Newpson::Parsing::obj::Parser::statusToString(status);
        qDebug() << "Aborting.";
        return -1;
    }

    return 0;
}
