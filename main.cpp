#include "mesh.hpp"
#include "obj/parser.hpp"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QVector3D>
#include <QVector2D>

int main()
{
    Newpson::Mesh mesh;
    Newpson::Parsing::OBJ::Parser::ParserState parserState;

    parserState = Newpson::Parsing::OBJ::Parser::load(PROJECT_ASSETS "/ok/nefertiti.obj", mesh);

    qDebug() << Newpson::Parsing::OBJ::Parser::statusToString(parserState.status) << "at";
    qDebug() << "line" << parserState.lineNumber;
    qDebug() << "column" << parserState.columnNumber;

    if (parserState.status == Newpson::Parsing::OBJ::Parser::STATUS_OK) {
        qDebug() << "Number of geometric vertices:" << mesh.geometryVertices.length();
        qDebug() << "Number of texture vertices:" << mesh.textureVertices.length();
        qDebug() << "Number of faces:" << mesh.geometryFaces.length();
    }


    return 0;
}
