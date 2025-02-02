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

    parserState = Newpson::Parsing::OBJ::Parser::load(PROJECT_ASSETS "/ok/cube-semifull.obj", mesh);

    qDebug() << Newpson::Parsing::OBJ::Parser::statusToString(parserState.status) << "at";
    qDebug() << "line" << parserState.lineNumber;
    qDebug() << "column" << parserState.columnNumber;

    if (parserState.status == Newpson::Parsing::OBJ::Parser::STATUS_OK) {
        qDebug() << "[Parsing stats]";
        qDebug() << "Number of geometric vertices:" << mesh.numGeometry();
        qDebug() << "Number of texture vertices:" << mesh.numTextures();
        qDebug() << "Number of normals:" << mesh.numNormals();
        qDebug() << "Number of faces:" << mesh.numFaces();
        qDebug() << "[Overall mesh data]";
        qDebug() << "geometry:" << mesh.geometry;
        qDebug() << "textures:" << mesh.textures;
        qDebug() << "normals:" << mesh.normals;
        qDebug() << "indicesGeometry:" << mesh.indicesGeometry;
        qDebug() << "indeicesTextures:" << mesh.indicesTextures;
        qDebug() << "indeicesNormals:" << mesh.indicesNormals;
        qDebug() << "facesGeometry:" << mesh.facesGeometry;
        qDebug() << "facesTextures:" << mesh.facesTextures;
        qDebug() << "facesNormals:" << mesh.facesNormals;
    }



    return 0;
}
