#ifndef NEWPSON_OBJ_PARSER_PARSERINTERNAL_H
#define NEWPSON_OBJ_PARSER_PARSERINTERNAL_H

#include "Newpson/Obj/Parser/parser.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::ObjParser::Internal {

enum LineType {
    LINETYPE_EMPTY,
    LINETYPE_COMMENT,
    LINETYPE_VERTEX,
    LINETYPE_VERTEX_TEXTURE,
    LINETYPE_NORMAL,
    LINETYPE_FACE,
    LINETYPE_GROUP,
    LINETYPE_UNKNOWN,
};

bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter);
bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);
bool hasMoreComponents(QChar const * const lineEnd, QChar const *&lineIter);
void skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
void skipUntilSlashOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

int indexMakeAbsolute(const int numComponents, const int index);

LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter);
Status parseFloat(QChar const * const lineEnd, QChar const *&lineIter, float &outFloat);
Status parseInteger(QChar const * const lineEnd, QChar const *&lineIter, int &outInteger);
Status parseVertexGeometric(QChar const * const lineEnd, QChar const *&lineIter, QVector3D &outVertex);
Status parseVertexTexture(QChar const * const lineEnd, QChar const *&lineIter, QVector2D &outVertex);
Status parseFace(
        const int numVerticesGeometric,
        const int numVerticesTexture,
        const int numNormals,
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<int> &outFaceGeometric,
        QVector<int> &outFaceTexture,
        QVector<int> &outFaceNormal);
Status parseFaceVertexComponents(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        int &indexGeometric,
        bool &hasIndexTexture,
        int &indexTexture,
        bool &hasIndexNormal,
        int &indexNormal);
Status parseGroupsNames(QChar const * const lineEnd, QChar const *&lineIter, QVector<QString> &groupsNames);

QVector3D generateNormal(
        const QVector<QVector3D> &vertices,
        const QVector<int> &indicesVertices,
        const int faceBegin,
        const int faceEnd);
QVector2D generateVerticesTexture();


}

#endif
