#include "Parser.hpp"
#include "Mesh.hpp"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::obj {

QString Parser::statusToString(Parser::Status const status)
{
    switch (status) {
    case STATUS_OK: return "OK";
    case STATUS_ERROR_STATE_BROKEN: return "State broken";
    case STATUS_ERROR_EXPECTED_FLOAT: return "Float expected";
    // Legacy errors/warns
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

bool Parser::skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    QChar const *begin = lineIter;
    while (lineIter < lineEnd && lineIter->isSpace())
        ++lineIter;
    bool hasSkipped = lineIter > begin;
    return hasSkipped;
}

void Parser::skipContent(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && (!lineIter->isSpace() || *lineIter != '/'))
        ++lineIter;
}

bool Parser::isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter)
{
    return (lineIter >= lineEnd || lineIter->isSpace());
}

bool Parser::isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    ++lineIter;
    return isEndOrSpace(lineEnd, lineIter);
}

Parser::LineType Parser::parseLineType(QChar const * const lineEnd, QChar const *&lineIter)
{
    if (lineIter < lineEnd) {
        if (*lineIter == '#') {
            return LINETYPE_COMMENT;
        } else if (*lineIter == 'v') {

            if (isNextCharEndOrSpace(lineEnd, lineIter))
                return LINETYPE_VERTEX_GEOMETRIC;

            if (*lineIter == 't') {

                if (isNextCharEndOrSpace(lineEnd, lineIter))
                    return LINETYPE_VERTEX_TEXTURE;

            } else if (*lineIter == 'n') {

                if (isNextCharEndOrSpace(lineEnd, lineIter))
                    return LINETYPE_NORMAL_VERTEX;
            }

        } else if (*lineIter == 'f') {
            if (isNextCharEndOrSpace(lineEnd, lineIter))
                return LINETYPE_FACE;
        }
    }

    return LINETYPE_UNKNOWN;
}

Parser::Status Parser::parseFloat(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        float &outFloat)
{
    QChar const * const contentBegin = lineIter;
    skipContent(lineEnd, lineIter);
    QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outFloat = content.toFloat(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin; // move cursor to the beginning of errorish value
        return STATUS_ERROR_EXPECTED_FLOAT;
    }

    return STATUS_OK;
}

Parser::Status Parser::parseInteger(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        float &outInteger)
{
    QChar const * const contentBegin = lineIter;
    skipContent(lineEnd, lineIter);
    QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outInteger = content.toInt(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin;
        return STATUS_ERROR_EXPECTED_INTEGER;
    }

    return STATUS_OK;
}

Parser::Status Parser::parseVertexGeometric(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector3D &outVertex)
{
    float coord = 0.0;
    Status status = STATUS_OK;

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_FLOAT;
    status = parseFloat(lineEnd, lineIter, coord);
    if (status != STATUS_OK)
        return status;
    outVertex.setX(coord);
    skipWhiteSpace(lineEnd, lineIter);

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_FLOAT;
    status = parseFloat(lineEnd, lineIter, coord);
    if (status != STATUS_OK)
        return status;
    outVertex.setY(coord);
    skipWhiteSpace(lineEnd, lineIter);

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_FLOAT;
    status = parseFloat(lineEnd, lineIter, coord);
    if (status != STATUS_OK)
        return status;
    outVertex.setZ(coord);
    skipWhiteSpace(lineEnd, lineIter);

    // if (!isEndOrSpace(lineEnd, lineIter))
    //     warnState(IgnoringExtraParameters);

    return STATUS_OK;
}

Parser::Status Parser::parseVertexTexture(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector2D &outVertex)
{
    float coord = 0.0;
    Status status = STATUS_OK;

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_FLOAT;
    status = parseFloat(lineEnd, lineIter, coord);
    if (status != STATUS_OK)
        return status;
    outVertex.setX(coord);
    skipWhiteSpace(lineEnd, lineIter);

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_FLOAT;
    status = parseFloat(lineEnd, lineIter, coord);
    if (status != STATUS_OK)
        return status;
    outVertex.setY(coord);
    skipWhiteSpace(lineEnd, lineIter);

    // if (!isEndOrSpace(lineEnd, lineIter))
    //     warnState(IgnoringExtraParameters);

    return STATUS_OK;
}

Parser::Status parseComponents(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        int &indexGeometric,
        bool &hasIndexTexture,
        int &indexTexture
        bool &hasIndexNormal,
        int &indexNormal)
{
    Status status = STATUS_OK;
    status = parseInteger(lineEnd, lineIter, indexGeometric);
    if (status != STATUS_OK)
        return status;
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = false;
        hasIndexNormal = false;
        return STATUS_OK;
    }

    ++lineIter; // skip '/'

    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = false;
        hasIndexNormal = false;
        return STATUS_OK;
    }
    status = parseInteger(lineEnd, lineIter, indexTexture);
    if (status != STATUS_OK)
        hasIndexTexture = false;
    if (isEndOrSpace(lineEnd, lineIter)) {
        return STATUS_OK;
    }
}

Parser::Status parseFace(
        int numVerticesGeometric,
        int numVerticesTexture,
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<int> &outFaceGeometric,
        QVector<int> &outFaceTexture)
{
    int indexGeometric = 0;
    int indexTexture = 0;

    if (lineIter >= lineEnd)
        return STATUS_ERROR_EXPECTED_INTEGER;

    Status status = STATUS_OK;
    status = parseComponents(lineEnd, lineIter, indexGeometric, indexTexture);
    if (status != STATUS_OK)
        return status;
}

Parser::Status Parser::load(QTextStream &input, Mesh &outMesh)
{
    for (auto &filePoints: warnings)
        filePoints.clear();

    int lineNumber = 0;
    while (!input.atEnd()) {
        QString const line = input.readLine();
        ++lineNumber;

        filePoint.line = lineNumber;

        if (line.isEmpty())
            continue;

        // TODO multiline statements support (when the line ends with '\')

        ctx_lineBegin = line.begin();

        QChar const *lineIter = line.begin(); // how does it cast?
        QChar const * const lineEnd = line.end();

        skipWhiteSpace(lineEnd, lineIter);
        // if (skipWhiteSpace(lineIter))
        //     warnStatus(WhiteSpaceTrimmed);

        LineType lineType = parseLineType(lineEnd, lineIter);
        skipWhiteSpace(lineEnd, lineIter);
        // we can't discard the line now if lineIter >= lineEnd (no arguments provided)
        // because there is single (!) token exists that does not take arguments.
        // may be add its support in the future :)
        // if (lineIter >= lineEnd)
        //     errorStatus(ExpectedArguments);

        Status status = STATUS_OK;
        switch (lineType) {
        case LINETYPE_COMMENT:
            continue;

        case LINETYPE_VERTEX_GEOMETRIC: {
            QVector3D vertexGeometric;
            status = parseVertexGeometric(lineEnd, lineIter, vertexGeometric);
            if (status != STATUS_OK) {
                filePoint.column = (lineIter - line.begin());
                return status;
            }
            outMesh.geometryVertices.append(vertexGeometric);
            break;
        }

        case LINETYPE_VERTEX_TEXTURE: {
            QVector2D vertexTexture;
            status = parseVertexTexture(lineEnd, lineIter, vertexTexture);
            if (status != STATUS_OK) {
                filePoint.column = (lineIter - line.begin());
                return status;
            }
            outMesh.textureVertices.append(vertexTexture);
            break;
        }

        case LINETYPE_NORMAL_VERTEX:
            // warnStatus(UnknownLineType);
            break;

        case LINETYPE_FACE: {
            break;
        }

        case LINETYPE_UNKNOWN:
            // warnStatus(UnknownLineType);
            break;

        default:
            return STATUS_ERROR_STATE_BROKEN;
        }
    }

    // if (lineNumber == 0)
    //     warnStatus(FileIsEmpty);

    return STATUS_OK;
}

Parser::FilePoint Parser::getFilePoint() const
{
    return filePoint;
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
