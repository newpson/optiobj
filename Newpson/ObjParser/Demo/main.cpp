#include "ObjParser/objparser.h"
#include "Newpson/Mesh/mesh.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QVector3D>
#include <QVector2D>

int main()
{
    Newpson::ObjParser::ParserResult parserResult;

    Newpson::Mesh mesh = Newpson::ObjParser::load(PROJECT_ASSETS "/ok/cube-semifull.obj", parserResult);
//     Newpson::Mesh mesh = Newpson::ObjParser::load(PROJECT_ASSETS "/ok/skull.obj", parserResult);

    qDebug() << Newpson::ObjParser::statusToString(parserResult.status) << "at";
    qDebug() << "line" << parserResult.lineNumber;
    qDebug() << "column" << parserResult.columnNumber;

    if (parserResult.status != Newpson::ObjParser::STATUS_OK)
        return -1;

    Newpson::Mesh::ValidationResult validationResult = mesh.checkConsistency();
    if (validationResult != Newpson::Mesh::VALIDATION_OK) {
        qDebug() << "Invalid mesh:" << validationResult;
        return -2;
    }

    qDebug() << "[Parsing stats]";
    qDebug() << "Number of vertices:" << mesh.vertices().length();
    qDebug() << "Number of texture vertices:" << mesh.verticesTexture().length();
    qDebug() << "Number of normals:" << mesh.normals().length();
    qDebug() << "Number of faces:" << mesh.facesEnds().length();
    qDebug() << "Number of groups:" << mesh.groupsEnds().length();
    qDebug() << "";
    qDebug() << "[Overall mesh data]";
    qDebug() << "vertices:" << mesh.vertices();
    qDebug() << "verticesTexture:" << mesh.verticesTexture();
    qDebug() << "normals:" << mesh.normals();
    qDebug() << "indicesVertices:" << mesh.indicesVertices();
    qDebug() << "indicesVerticesTexture:" << mesh.indicesVerticesTexture();
    qDebug() << "indicesNormals:" << mesh.indicesNormals();
    qDebug() << "facesEnds:" << mesh.facesEnds();
    qDebug() << "groupsNames:" << mesh.groupsNames();
    qDebug() << "groupsEnds:" << mesh.groupsEnds();
    qDebug() << "";

    qDebug() << "[Triangulation]";
    Newpson::Mesh triMesh = mesh.triangulate();
    qDebug() << "[Stats]";
    qDebug() << "Number of vertices:" << triMesh.vertices().length();
    qDebug() << "Number of texture vertices:" << triMesh.verticesTexture().length();
    qDebug() << "Number of normals:" << triMesh.normals().length();
    qDebug() << "Number of faces:" << triMesh.facesEnds().length();
    qDebug() << "Number of groups:" << triMesh.groupsEnds().length();
    qDebug() << "";
    qDebug() << "[Overall mesh data]";
    qDebug() << "vertices:" << triMesh.vertices();
    qDebug() << "verticesTexture:" << triMesh.verticesTexture();
    qDebug() << "normals:" << triMesh.normals();
    qDebug() << "indicesVertices:" << triMesh.indicesVertices();
    qDebug() << "indicesVerticesTexture:" << triMesh.indicesVerticesTexture();
    qDebug() << "indicesNormals:" << triMesh.indicesNormals();
    qDebug() << "facesEnds:" << triMesh.facesEnds();
    qDebug() << "groupsNames:" << triMesh.groupsNames();
    qDebug() << "groupsEnds:" << triMesh.groupsEnds();
    return 0;
}
