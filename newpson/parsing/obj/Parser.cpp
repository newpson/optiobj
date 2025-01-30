#include "Parser.hpp"
#include "Mesh.hpp"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::obj {

QString Parser::statusToString(Parser::Status const status)
{
    switch (status) {
    case STATUS_OK: return "OK";
    case STATUS_WARN_IO_EMPTY: return "Opened file is empty";
    case STATUS_WARN_UNKNOWN_DATATYPE: return "Unknown data type";
    case STATUS_ERROR_IO: return "Input/output error";
    case STATUS_ERROR_INVALID_FLOAT_X: return "Float parsing error (x coordinate)";
    case STATUS_ERROR_INVALID_FLOAT_Y: return "Float parsing error (y coordinate)";
    case STATUS_ERROR_INVALID_FLOAT_Z: return "Float parsing error (z coordinate)";
    case STATUS_ERROR_INVALID_PARAMETERS_VERTEX_GEOMETRY: return "Too few points to parse a geometry vertex";
    case STATUS_ERROR_INVALID_PARAMETERS_VERTEX_TEXTURE: return "Too few points to parse a texture vertex";
    case STATUS_ERROR_INVALID_PARAMETERS_FACE: return "Too few points to parse a face";
    case STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS: return "Ill-formed components in face vertices";
    case STATUS_ERROR_INVALID_INDEX_VERTEX_GEOMETRY: return "Invalid geometry vertex index";
    case STATUS_ERROR_INVALID_INDEX_VERTEX_TEXTURE: return "Invalid texture vertex index";
    default: return "Reserved state";
    }
}

bool Parser::isComment(QString const &line)
{
    Q_ASSERT(!line.isEmpty());
    return line[0] == CHAR_COMMENT;
}

Parser::Status Parser::statusType(Parser::Status const status)
{
    bool const isVerbose = (status > STATUS_DEBUG_BEGIN && status < STATUS_DEBUG_END);
    if (isVerbose)
        return STATUS_VERBOSE;

    bool const isWarning = (status > STATUS_DEBUG_BEGIN && status < STATUS_DEBUG_END);
    if (isWarning)
        return STATUS_WARN;

    bool const isError = (status > STATUS_ERROR_BEGIN && status < STATUS_ERROR_END);
    if (isError)
        return STATUS_ERROR;

    return STATUS_RESERVED;
}

Parser::Status Parser::parseGeometryVertex(QStringList const &tokens, QVector3D &outVertex)
{
    if (tokens.length() < LENGTH_MIN_DATATYPE_VERTEX_GEOMETRY)
        return STATUS_ERROR_INVALID_PARAMETERS_VERTEX_GEOMETRY;

    bool parseSuccess;
    outVertex.setX(tokens[INDEX_TOKEN_VERTEX_X].toFloat(&parseSuccess));
    if (!parseSuccess)
        return STATUS_ERROR_INVALID_FLOAT_X;
    outVertex.setY(tokens[INDEX_TOKEN_VERTEX_Y].toFloat(&parseSuccess));
    if (!parseSuccess)
        return STATUS_ERROR_INVALID_FLOAT_Y;
    outVertex.setY(tokens[INDEX_TOKEN_VERTEX_Z].toFloat(&parseSuccess));
    if (!parseSuccess)
        return STATUS_ERROR_INVALID_FLOAT_Z;

    return STATUS_OK;
}

Parser::Status Parser::parseTextureVertex(QStringList const &tokens, QVector2D &outVertex)
{
    if (tokens.length() < LENGTH_MIN_DATATYPE_VERTEX_TEXTURE)
        return STATUS_ERROR_INVALID_PARAMETERS_VERTEX_GEOMETRY;

    bool parseSuccess;

    outVertex.setX(tokens[INDEX_TOKEN_VERTEX_X].toFloat(&parseSuccess));
    if (!parseSuccess)
        return STATUS_ERROR_INVALID_FLOAT_X;

    outVertex.setY(tokens[INDEX_TOKEN_VERTEX_Y].toFloat(&parseSuccess));
    if (!parseSuccess)
        return STATUS_ERROR_INVALID_FLOAT_Y;

    return STATUS_OK;
}

// TODO refactor (change logic)
bool Utility::hasStateChanged(bool nextState, bool reset)
{
    static bool state = false;
    static bool isMutable = false;

    if (reset) {
        isMutable = true;
        return false;
    }
    else if (isMutable) {
        state = nextState;
        isMutable = false;
    }

    return (state != nextState);
}

Parser::Status Parser::parseFace(
    int const numGeometryVertices,
    int const numTextureVertices,
    QStringList const &tokens,
    QVector<int> &outGeometryFace,
    QVector<int> &outTextureFace)
{
    if (tokens.length() < LENGTH_MIN_DATATYPE_FACE)
        return STATUS_ERROR_INVALID_PARAMETERS_FACE;

    bool hasTextureVertices = false;
    Utility::hasStateChanged(hasTextureVertices, true);
    for (auto triadIter = tokens.begin() + 1; triadIter != tokens.end(); ++triadIter) {
        const QString &triad = *triadIter;

        QStringList components = triad.split(CHAR_DELIMITER);
        if (components.length() < LENGTH_MIN_VERTEX_COMPONENTS)
            return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;

        bool isParsedSuccessfuly = false;
        int indexGeometry = 0;
        int indexTexture = 0;
        if (!components[INDEX_TOKEN_VERTEX_COMPONENT_GEOMETRY].isEmpty()) {
            indexGeometry = components[INDEX_TOKEN_VERTEX_COMPONENT_GEOMETRY].toUInt(&isParsedSuccessfuly);
            if (!isParsedSuccessfuly)
                return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
            if (indexGeometry > numGeometryVertices)
                return STATUS_ERROR_INVALID_INDEX_VERTEX_GEOMETRY;
            outGeometryFace.append(indexGeometry);
        } else {
            return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
        }

        if (components.length() > LENGTH_MIN_VERTEX_COMPONENTS) {
            if (!components[INDEX_TOKEN_VERTEX_COMPONENT_TEXTURE].isEmpty()) {
                if (Utility::hasStateChanged((hasTextureVertices = true)))
                    return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
                indexTexture = components[INDEX_TOKEN_VERTEX_COMPONENT_TEXTURE].toUInt(&isParsedSuccessfuly);
                if (!isParsedSuccessfuly)
                    return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
                if (indexTexture > numTextureVertices)
                    return STATUS_ERROR_INVALID_INDEX_VERTEX_TEXTURE;
                outTextureFace.append(indexTexture);
            } else {
                if (Utility::hasStateChanged((hasTextureVertices = false)))
                    return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
            }
        }
    }

    return STATUS_OK;
}

Parser::Status Parser::evaluateTokens(QStringList const &tokens, Mesh &outMesh)
{
    Parser::Status status = STATUS_OK;
    QString const &datatype = tokens[INDEX_TOKEN_DATATYPE];

    if (datatype[0] == 'v') {
        if (datatype.length() > 1) {
            if (datatype[1] == 't') {
                QVector2D vertex;
                status = parseTextureVertex(tokens, vertex);
                if (status <= STATUS_ERROR) {
                    outMesh.textureVertices.append(vertex);
                    qDebug() << vertex;
                }
            }
        } else {
            QVector3D vertex;
            status = parseGeometryVertex(tokens, vertex);
            // #2: make wrapper as below in read()
            if (status <= STATUS_ERROR) {
                outMesh.geometryVertices.append(vertex);
                qDebug() << vertex;
            }
        }
    } else if (datatype[0] == 'f') {
        QVector<int> geometryFace, textureFace;
        status = parseFace(
            outMesh.geometryVertices.length(),
            outMesh.textureVertices.length(),
            tokens,
            geometryFace,
            textureFace);
        // #2: make wrapper as below in read()
        if (status <= STATUS_ERROR) {
            outMesh.geometryFaces.append(geometryFace);
            outMesh.textureFaces.append(textureFace);
            qDebug() << geometryFace << ";" << textureFace;
        }
    } else {
        status = STATUS_WARN_UNKNOWN_DATATYPE;
    }

    return status;
}

Parser::Status Parser::load(QTextStream &input, Mesh &outMesh)
{
    while (!input.atEnd()) {
        QString const line = input.readLine();
        if (line.isEmpty() || isComment(line))
            continue;

        QStringList tokens = line.split(CHAR_SPACE);

        Status status = evaluateTokens(tokens, outMesh);
        // #2: like here
        if (statusType(status) == STATUS_WARN)
            qDebug() << statusToString(status);
        // #2:
        else if (status >= STATUS_ERROR)
            return status;
    }
    return STATUS_OK;
}

Parser::Status Parser::load(QTextStream &&input, Mesh &outMesh)
{
    return load(input, outMesh);
}

Parser::Status Parser::load(QString const &filename, Mesh &outMesh)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return STATUS_ERROR_IO;

    QTextStream input(&file);
    if (input.atEnd())
        return STATUS_WARN_IO_EMPTY;

    return load(input, outMesh);
}

} // namespace Newpson::Parser::OBJ
