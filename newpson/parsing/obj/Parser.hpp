#ifndef NEWPSON_PARSING_OBJ_PARSER_H
#define NEWPSON_PARSING_OBJ_PARSER_H

#include "Mesh.hpp"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>


//! REVIEW Obj
namespace Newpson::Parsing::obj {

class Parser
{
public:
    enum LineType {
        LINETYPE_COMMENT,
        LINETYPE_VERTEX_GEOMETRIC,
        LINETYPE_VERTEX_TEXTURE,
        // there is no any type of normals in .obj beside vertex normals; should we call this type just LINETYPE_NORMAL?
        LINETYPE_NORMAL_VERTEX,
        LINETYPE_FACE,
        LINETYPE_UNKNOWN,
    };

    enum Status {
        STATUS_VERBOSE,
        STATUS_DEBUG_BEGIN,
        STATUS_OK,
        STATUS_DEBUG_END,

        STATUS_WARN,
        STATUS_WARN_BEGIN,
        STATUS_WARN_IO_EMPTY,
        STATUS_WARN_UNKNOWN_DATATYPE,
        STATUS_WARN_END,

        STATUS_ERROR,
        STATUS_ERROR_BEGIN,
        STATUS_ERROR_STATE_BROKEN,
        STATUS_ERROR_EXPECTED_FLOAT,
        STATUS_ERROR_EXPECTED_INTEGER,
        STATUS_ERROR_INPUT,
        STATUS_ERROR_INPUT_EMPTY,

        // Legacy errors
        STATUS_ERROR_IO,
        STATUS_ERROR_INVALID_FLOAT_X,
        STATUS_ERROR_INVALID_FLOAT_Y,
        STATUS_ERROR_INVALID_FLOAT_Z,
        STATUS_ERROR_INVALID_PARAMETERS_VERTEX_GEOMETRY,
        STATUS_ERROR_INVALID_PARAMETERS_VERTEX_TEXTURE,
        STATUS_ERROR_INVALID_PARAMETERS_FACE,
        STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS,
        STATUS_ERROR_INVALID_INDEX_VERTEX_GEOMETRY,
        STATUS_ERROR_INVALID_INDEX_VERTEX_TEXTURE,
        STATUS_ERROR_END,

        STATUS_RESERVED,
    };

    struct ParserStatus
    {
        Status status;
        int lineNumber;
        int columnNumber;

        ParserStatus();
        ParserStatus(Status status);
    };

    static QString statusToString(Status status);
    static Status statusType(Status const status);

    static ParserStatus load(QTextStream &input, Mesh &outMesh);
    static ParserStatus load(QTextStream &&input, Mesh &outMesh);
    static ParserStatus load(QString const &filename, Mesh &outMesh);

private:
    static bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter);

    static bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
    static void skipContent(QChar const * const lineEnd, QChar const *&lineIter);
    static bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

    static LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter);
    static Status parseFloat(QChar const * const lineEnd, QChar const *&lineIter, float &outFloat);
    static Status parseVertexGeometric(QChar const * const lineEnd, QChar const *&lineIter, QVector3D &outVertex);
    static Status parseVertexTexture(QChar const * const lineEnd, QChar const *&lineIter, QVector2D &outVertex);
    static Status parseFace(
            int numVerticesGeometric,
            int numVerticesTexture,
            QChar const * const lineEnd,
            QChar const *&lineIter,
            QVector<int> &outFaceGeometric,
            QVector<int> &outFaceTexture);
};

} // namespace Newpson::Parsing::obj

#endif // NEWPSON_PARSING_OBJ_PARSER_H
