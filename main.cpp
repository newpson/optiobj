#include "Mesh.hpp"
#include "obj/Parser.hpp"

#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>
#include <QTextStream>

int main()
{
    Newpson::Mesh mesh;
    Newpson::Parsing::obj::Parser::ParserState parserState;

    parserState = Newpson::Parsing::obj::Parser::load(PROJECT_ASSETS "/ok/nefertiti.obj", mesh);

    qDebug() << Newpson::Parsing::obj::Parser::statusToString(parserState.status) << "at";
    qDebug() << "line" << parserState.lineNumber;
    qDebug() << "column" << parserState.columnNumber;

    if (parserState.status == Newpson::Parsing::obj::Parser::STATUS_OK) {
        qDebug() << "Number of geometric vertices:" << mesh.geometryVertices.length();
        qDebug() << "Number of texture vertices:" << mesh.textureVertices.length();
        qDebug() << "Number of faces:" << mesh.geometryFaces.length();
    }


    return 0;
}
