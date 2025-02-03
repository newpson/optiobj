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
    Newpson::Parsing::Obj::ParserResult parserResult;

    Newpson::Mesh mesh = Newpson::Parsing::Obj::load(PROJECT_ASSETS "/ok/cube.obj", parserResult);

    qDebug() << Newpson::Parsing::Obj::statusToString(parserResult.status) << "at";
    qDebug() << "line" << parserResult.lineNumber;
    qDebug() << "column" << parserResult.columnNumber;

    if (parserResult.status == Newpson::Parsing::Obj::STATUS_OK) {
        qDebug() << "[Parsing stats]";
        qDebug() << "Number of geometric vertices:" << mesh.vertices().length();
        qDebug() << "Number of texture vertices:" << mesh.verticesTexture().length();
        qDebug() << "Number of normals:" << mesh.normals().length();
        qDebug() << "Number of faces:" << mesh.facesVertices().length();
    }
    //        qDebug() << "[Overall mesh data]";
    //        qDebug() << "geometry:" << mesh.geometry;
    //        qDebug() << "textures:" << mesh.textures;
    //        qDebug() << "normals:" << mesh.normals;
    //        qDebug() << "indicesGeometry:" << mesh.indicesGeometry;
    //        qDebug() << "indeicesTextures:" << mesh.indicesTextures;
    //        qDebug() << "indeicesNormals:" << mesh.indicesNormals;
    //        qDebug() << "facesGeometry:" << mesh.facesGeometry;
    //        qDebug() << "facesTextures:" << mesh.facesTextures;
    //        qDebug() << "facesNormals:" << mesh.facesNormals;
    //    }

    return 0;
}
