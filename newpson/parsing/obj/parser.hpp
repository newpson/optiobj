#ifndef NEWPSON_PARSING_OBJ_PARSER_H
#define NEWPSON_PARSING_OBJ_PARSER_H

#include "mesh.hpp"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::Obj {

enum Status {
    STATUS_VERBOSE,
    STATUS_DEBUG_BEGIN,
    STATUS_OK,
    STATUS_DEBUG_END,

    STATUS_ERROR,
    STATUS_ERROR_BEGIN,
    STATUS_ERROR_STATE_BROKEN,
    STATUS_ERROR_EXPECTED_FLOAT,
    STATUS_ERROR_EXPECTED_INTEGER,
    STATUS_ERROR_COMPONENTS_INCOHERENCE,
    STATUS_ERROR_UNDEFINED_INDEX,
    STATUS_ERROR_INPUT,
    STATUS_ERROR_INPUT_EMPTY,
    STATUS_ERROR_END,

    STATUS_RESERVED,
};

struct ParserResult
{
    Status status;
    int lineNumber;
    int columnNumber;

    ParserResult();
    ParserResult(Status status);
};

QString statusToString(Status status);
Status statusType(Status const status);

Mesh load(QTextStream &input, ParserResult &parserResult);
Mesh load(QTextStream &&input, ParserResult &parserResult);
Mesh load(QString const &filename, ParserResult &parserResult);

namespace Internal {

enum LineType {
    LINETYPE_EMPTY,
    LINETYPE_COMMENT,
    LINETYPE_VERTEX_GEOMETRIC,
    LINETYPE_VERTEX_TEXTURE,
    LINETYPE_NORMAL,
    LINETYPE_FACE,
    LINETYPE_UNKNOWN,
};

bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter);
bool hasMoreComponents(QChar const * const lineEnd, QChar const *&lineIter);
bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
void skipUntilDelimiter(QChar const * const lineEnd, QChar const *&lineIter);
void skipUntilContent(QChar const * const lineEnd, QChar const *&lineIter);
bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

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
    bool &hasVerticesTexture,
    bool &hasNormals,
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

} // namespace Internal

} // namespace Newpson::Parsing::obj

#endif // NEWPSON_PARSING_OBJ_PARSER_H
