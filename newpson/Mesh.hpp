#ifndef NEWPSON_MESH_H
#define NEWPSON_MESH_H

#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>

namespace Newpson {

class Mesh
{
public:
    enum Status {
        STATUS_VERBOSE,
        STATUS_VERBOSE_BEGIN,
        STATUS_OK,
        STATUS_VERBOSE_END,

        STATUS_WARN,
        STATUS_WARN_BEGIN,
        STATUS_WARN_IO_EMPTY,
        STATUS_WARN_UNKNOWN_DATATYPE,
        STATUS_WARN_END,

        STATUS_ERROR,
        STATUS_ERROR_BEGIN,
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

//  #1: "sorry, unimplemented: non-trivial designated initializers not supported"
//    static constexpr char const *statusStrings[] = {
//        [STATUS_OK] = "OK",
//        [STATUS_ERROR_IO] = "Input/output error",
//        [STATUS_WARN_IO_EMPTY] = "Opened file is empty",
//    };

    Mesh();

    static char const * statusToString(Status status);
    static Status statusType(Status status);

    static bool isComment(QString &line);
    static Status parseGeometryVertex(QVector3D &outVertex, QStringList &tokens);
    static Status parseTextureVertex(QVector2D &outVertex, QStringList &tokens);
    static Status parseFace(
            int numGeometryVertices,
            int numTextureVertices,
            QVector<int> &outGeometryFace,
            QVector<int> &outTextureFace,
            QStringList &tokens);
    static Status evaluateTokens(Mesh &outMesh, QStringList &tokens);

    static Status read(Mesh &outMesh, QTextStream &input);
    static Status read(Mesh &outMesh, QTextStream &&input);
    static Status read(Mesh &outMesh, QString &filename);
    static Status read(Mesh &outMesh, QString &&filename);
    static Status read(Mesh &outMesh, char const *filename);

protected:
    static constexpr QChar CHAR_COMMENT = '#';
    static constexpr QChar CHAR_SPACE = ' ';
    static constexpr QChar CHAR_DELIMITER = '/';

    QVector<QVector3D> geometryVertices;
    QVector<QVector<int>> geometryFaces;
    QVector<QVector2D> textureVertices;
    QVector<QVector<int>> textureFaces;

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
bool hasChanged(bool nextState, bool reset = false);

} // namespace Utility

} // namespace Newpson

#endif // NEWPSON_MESH_H
