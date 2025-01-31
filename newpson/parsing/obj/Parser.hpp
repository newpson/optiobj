#ifndef NEWPSON_PARSING_OBJ_PARSER_H
#define NEWPSON_PARSING_OBJ_PARSER_H

#include "Mesh.hpp"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

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

    static QString statusToString(Status status);
    static Status statusType(Status const status);

    // static Status load(QTextStream &input, Mesh &outMesh);
    // static Status load(QTextStream &&input, Mesh &outMesh);
    // static Status load(QString const &filename, Mesh &outMesh);
    Status load(QTextStream &input, Mesh &outMesh);
    Status load(QTextStream &&input, Mesh &outMesh);
    Status load(QString const &filename, Mesh &outMesh);

    struct FilePoint
    {
        int line;
        int column;
    };

    FilePoint getFilePoint() const;

protected:
    static constexpr int numWarnCodes = (STATUS_WARN_END - STATUS_WARN_BEGIN - 1);
    static constexpr int numErrorsCodes = (STATUS_WARN_END - STATUS_WARN_BEGIN - 1);
    // QPoint is too complex for this
    std::array<QVector<FilePoint>, numWarnCodes> warnings;
    QChar const *ctx_lineBegin; // to calculate column when a warning comes, can be join into structure with lineIter and lineEnd
    FilePoint filePoint;

    bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter);

    bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter);
    void skipContent(QChar const * const lineEnd, QChar const *&lineIter);
    bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter);

    LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter);
    Status parseFloat(QChar const * const lineEnd, QChar const *&lineIter, float &outFloat);
    Status parseVertexGeometric(QChar const * const lineEnd, QChar const *&lineIter, QVector3D &outVertex);
    Status parseVertexTexture(QChar const * const lineEnd, QChar const *&lineIter, QVector2D &outVertex);
    Status parseFace(
            int numVerticesGeometric,
            int numVerticesTexture,
            QChar const * const lineEnd,
            QChar const *&lineIter,
            QVector<int> &outFaceGeometric,
            QVector<int> &outFaceTexture);




    // Legacy

    enum ParsingConstants {
        LENGTH_MIN_DATATYPE_VERTEX_GEOMETRY = 1 + 3, // DATATYPE VERTEX_X VERTEX_Y VERTEX_Z
        LENGTH_MIN_DATATYPE_VERTEX_TEXTURE = 1 + 2, // DATATYPE VERTEX_X VERTEX_Y
        LENGTH_MIN_DATATYPE_FACE = 1 + 3, // DATATYPE VERTEX_1 VERTEX_2 VERTEX_3 ...
        LENGTH_MIN_VERTEX_COMPONENTS = 1, // COMPONENT_GEOMETRY

        INDEX_TOKEN_DATATYPE = 0,
        INDEX_TOKEN_VERTEX_X = 1,
        INDEX_TOKEN_VERTEX_Y = 2,
        INDEX_TOKEN_VERTEX_Z = 3,
        INDEX_TOKEN_VERTEX_COMPONENT_GEOMETRY = 0,
        INDEX_TOKEN_VERTEX_COMPONENT_TEXTURE = 1,
    };

    static constexpr QChar CHAR_COMMENT = '#';
    static constexpr QChar CHAR_SPACE = ' ';
    static constexpr QChar CHAR_DELIMITER = '/';

    static bool isComment(QString const &line);
    static Status parseGeometryVertex(QStringList const &tokens, QVector3D &outVertex);
    static Status parseTextureVertex(QStringList const &tokens, QVector2D &outVertex);
    static Status parseFace_legacy(
            int const numGeometryVertices,
            int const numTextureVertices,
            QStringList const &tokens,
            QVector<int> &outGeometryFace,
            QVector<int> &outTextureFace);
    // TODO rename
    static Status evaluateTokens(QStringList const &tokens, Mesh &outMesh);
};


namespace Utility {

/* How does it work
 *
 * hasChanged(anystate, true); // reset
 * hasChanged(state); // set state that should be immutable
 * hasChanged(state) // check if state changed
 * hasChanged(state) // check if state changed
 * ...
 */
bool hasStateChanged(bool nextState, bool reset = false);

} // namespace Utility

} // namespace Newpson::Paser::OBJ

#endif // NEWPSON_PARSING_OBJ_PARSER_H
