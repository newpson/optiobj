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
    //! REVIEW: инвертировать внешний иф, чтобы была меньше вложенность
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

Parser::Status parseFaceVertexComponents(
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

Parser::ParserStatus Parser::load(QTextStream &input, Mesh &outMesh)
{
    ParserStatus parserStatus;

    while (!input.atEnd()) {
        const QString line = input.readLine();
        ++parserStatus.lineNumber;

        if (line.isEmpty())
            continue;

        // TODO multiline statements support (when the line ends with '\')

        QChar const *lineIter = line.begin();
        QChar const * const lineEnd = line.end();

        skipWhiteSpace(lineEnd, lineIter);
        const LineType lineType = parseLineType(lineEnd, lineIter);
        skipWhiteSpace(lineEnd, lineIter);

        switch (lineType) {
        case LINETYPE_COMMENT:
            continue;

        case LINETYPE_VERTEX_GEOMETRIC: {
            QVector3D vertexGeometric;
            parserStatus.status = parseVertexGeometric(lineEnd, lineIter, vertexGeometric);
            if (parserStatus.status != STATUS_OK)
                return parserStatus;
            outMesh.geometryVertices.append(vertexGeometric);
            break;
        }

        case LINETYPE_VERTEX_TEXTURE: {
            QVector2D vertexTexture;
            parserStatus.status = parseVertexTexture(lineEnd, lineIter, vertexTexture);
            if (parserStatus.status != STATUS_OK)
                return parserStatus;
            outMesh.textureVertices.append(vertexTexture);
            break;
        }

        case LINETYPE_NORMAL_VERTEX: {
//            QVector3D normal;
//            parserStatus.status = parseVertexGeometric(lineEnd, lineIter, normal);
//            if (parserStatus.status != STATUS_OK)
//                return parserStatus;
//            outMesh.normals.append(normal);
            break;
        }

        case LINETYPE_FACE: {
            QVector<int> faceGeometric;
            QVector<int> faceTexture;
            parserStatus.status = parseFace(
                    outMesh.geometryVertices.length(),
                    outMesh.textureVertices.length(),
                    lineEnd, lineIter,
                    faceGeometric, faceTexture);
            if (parserStatus.status != STATUS_OK)
                return parserStatus;
            outMesh.geometryFaces.append(faceGeometric);
            outMesh.textureFaces.append(faceTexture);
            break;
        }

        case LINETYPE_UNKNOWN:
            break;

        default:
            return STATUS_ERROR_STATE_BROKEN;
        }
    }

    return STATUS_OK;
}


Parser::ParserStatus::ParserStatus(Status status):
        lineNumber(0), columnNumber(0), status(status)
{}

Parser::ParserStatus::ParserStatus():
        ParserStatus(STATUS_OK)
{}

Parser::ParserStatus Parser::load(QTextStream &&input, Mesh &outMesh)
{
    return load(input, outMesh);
}

Parser::ParserStatus Parser::load(QString const &filename, Mesh &outMesh)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ParserStatus(STATUS_ERROR_INPUT);

    return load(input, outMesh);
}

} // namespace Newpson::Parser::OBJ
