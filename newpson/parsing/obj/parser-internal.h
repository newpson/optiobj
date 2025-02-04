#ifndef NEWPSON_PARSING_OBJ_PARSER_INTERNAL_H
#define NEWPSON_PARSING_OBJ_PARSER_INTERNAL_H

#include "mesh.hpp"
#include "parser.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::Obj::Internal {

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
bool hasMoreComponents(QChar const * const lineEnd, QChar const *&lineIter);
bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
void skipUntilDelimiter(QChar const * const lineEnd, QChar const *&lineIter);
void skipUntilContent(QChar const * const lineEnd, QChar const *&lineIter);
bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

void switchActiveGroups(
    const int numFaces,
    const int numNextActiveGroups,
    QVector<int> &groupsBegins,
    QVector<int> &groupsEnds);
void fillRemainingGroupsEnds(
    const int numFaces,
    const int numActiveGroups,
    QVector<int> &groupsEnds);

LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter);
Status parseFloat(QChar const * const lineEnd, QChar const *&lineIter, float &outFloat);
Status parseInteger(QChar const * const lineEnd, QChar const *&lineIter, int &outInteger);
Status parseVertexGeometric(QChar const * const lineEnd, QChar const *&lineIter, QVector3D &outVertex);
Status parseVertexTexture(QChar const * const lineEnd, QChar const *&lineIter, QVector2D &outVertex);
Status parsePolygroup(QChar const * const lineEnd, QChar const *&lineIter, QString &outName);
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

} // namespace Newpson::Parsing::Obj::Internal

#endif // NEWPSON_PARSING_OBJ_PARSER_INTERNAL_H
