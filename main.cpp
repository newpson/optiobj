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
    Newpson::Parsing::obj::Parser parser;

    // auto status = parser.load(PROJECT_ASSETS "/error/invalid-indices/004.obj", mesh);
    auto status = parser.load(PROJECT_ASSETS "/ok/cube.obj", mesh);

    if (status != Newpson::Parsing::obj::Parser::STATUS_OK) {
        Newpson::Parsing::obj::Parser::FilePoint filePoint = parser.getFilePoint();
        qDebug() << Newpson::Parsing::obj::Parser::statusToString(status) << "at";
        qDebug() << "line" << filePoint.line;
        qDebug() << "column" << filePoint.column;
    } else {
        qDebug() << "Everything is fine";
    }

    return 0;
}
