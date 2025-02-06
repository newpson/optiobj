#include "Newpson/mesh.h"
#include "Obj/parser.h"

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

    // Newpson::Mesh mesh = Newpson::Parsing::Obj::load(PROJECT_ASSETS "/ok/cube-semifull.obj", parserResult);
     Newpson::Mesh mesh = Newpson::Parsing::Obj::load(PROJECT_ASSETS "/ok/skull.obj", parserResult);
//    Newpson::Mesh mesh = Newpson::Parsing::Obj::load(QTextStream(
//                                                        "v 1 1 1 \n"
//                                                        "vt 1 1 1 \n"
//                                                        "vn 1 1 1 \n"
//                                                        "f 1/1/1 \\\n"
//                                                        "  1/1/1 \\\n"
//                                                        "  1/1/1 \\\n"), parserResult);

    qDebug() << Newpson::Parsing::Obj::statusToString(parserResult.status) << "at";
    qDebug() << "line" << parserResult.lineNumber;
    qDebug() << "column" << parserResult.columnNumber;

    if (parserResult.status == Newpson::Parsing::Obj::STATUS_OK) {
        if (!mesh.areLengthsOfFacesIndicesVecotorsEqualAndIndicesVerticesVectorIsValidAndIndicesVerticesTextureVectorIsValidAndIndicesNormalsVectorIsValidAndFaceVerticesIndicesValidAndFaceVerticesTextureInidicesValidAndEtc())
            qDebug() << "Invalid mesh.";
        else
            qDebug() << "Mesh is valid.";
        qDebug() << "[Parsing stats]";
        qDebug() << "Number of geometric vertices:" << mesh.vertices().length();
        qDebug() << "Number of texture vertices:" << mesh.verticesTexture().length();
        qDebug() << "Number of normals:" << mesh.normals().length();
        qDebug() << "Number of faces:" << mesh.facesVertices().length();

//        qDebug() << "[Overall mesh data]";
//        qDebug() << "geometry:" << mesh.vertices();
//        qDebug() << "textures:" << mesh.verticesTexture();
//        qDebug() << "normals:" << mesh.normals();
//        qDebug() << "indicesGeometry:" << mesh.indicesVertices();
//        qDebug() << "indicesTextures:" << mesh.indicesVerticesTexture();
//        qDebug() << "indicesNormals:" << mesh.indicesNormals();
//        qDebug() << "facesGeometry:" << mesh.facesVertices();
//        qDebug() << "facesTextures:" << mesh.facesVerticesTexture();
//        qDebug() << "facesNormals:" << mesh.facesNormals();
        qDebug() << "groupsNames:" << mesh.groupsNames();
        qDebug() << "groupsBegins:" << mesh.groupsBegins();
        qDebug() << "groupsEnds:" << mesh.groupsEnds();
    }



    return 0;
}
