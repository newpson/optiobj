#ifndef NEWPSON_PARSING_OBJ_PARSER_H
#define NEWPSON_PARSING_OBJ_PARSER_H

#include "mesh.hpp"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::OBJ {

class Parser
{
public:
    enum LineType {
        LINETYPE_EMPTY,
        LINETYPE_COMMENT,
        LINETYPE_VERTEX_GEOMETRIC,
        LINETYPE_VERTEX_TEXTURE,
        LINETYPE_NORMAL,
        LINETYPE_FACE,
        LINETYPE_UNKNOWN,
    };

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
        STATUS_ERROR_COMPONENTS_ASSYMETRY,
        STATUS_ERROR_UNDEFINED_INDEX,
        STATUS_ERROR_INPUT,
        STATUS_ERROR_INPUT_EMPTY,
        STATUS_ERROR_END,

        STATUS_RESERVED,
    };

    struct ParserState
    {
        Status status;
        int lineNumber;
        int columnNumber;

        ParserState();
        ParserState(Status status);
    };

    static QString statusToString(Status status);
    static Status statusType(Status const status);

    static ParserState load(QTextStream &input, Mesh &outMesh);
    static ParserState load(QTextStream &&input, Mesh &outMesh);
    static ParserState load(QString const &filename, Mesh &outMesh);

private:
    static bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter);
    static bool hasMoreComponents(QChar const * const lineEnd, QChar const *&lineIter);
    static bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
    static void skipUntilDelimiter(QChar const * const lineEnd, QChar const *&lineIter);
    static void skipUntilContent(QChar const * const lineEnd, QChar const *&lineIter);
    static bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

    static LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter);
    static Status parseFloat(QChar const * const lineEnd, QChar const *&lineIter, float &outFloat);
    static Status parseInteger(QChar const * const lineEnd, QChar const *&lineIter, int &outInteger);
    static Status parseVertexGeometric(QChar const * const lineEnd, QChar const *&lineIter, QVector3D &outVertex);
    static Status parseVertexTexture(QChar const * const lineEnd, QChar const *&lineIter, QVector2D &outVertex);
    static Status parseFace(
            int numVerticesGeometric,
            int numVerticesTexture,
            QChar const * const lineEnd,
            QChar const *&lineIter,
            QVector<int> &outFaceGeometric,
            QVector<int> &outFaceTexture,
            QVector<int> &outFaceNormal);
    static Status parseFaceVertexComponents(
            QChar const * const lineEnd,
            QChar const *&lineIter,
            int &indexGeometric,
            bool &hasIndexTexture,
            int &indexTexture,
            bool &hasIndexNormal,
            int &indexNormal);
};

} // namespace Newpson::Parsing::obj

#endif // NEWPSON_PARSING_OBJ_PARSER_H
