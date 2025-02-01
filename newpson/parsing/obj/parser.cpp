#include "parser.hpp"
#include "mesh.hpp"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::Parsing::OBJ {

QString Parser::statusToString(Parser::Status const status)
{
    switch (status) {
    case STATUS_OK: return "OK";
    case STATUS_ERROR_STATE_BROKEN: return "State broken";
    case STATUS_ERROR_EXPECTED_FLOAT: return "Float expected";
    case STATUS_ERROR_EXPECTED_INTEGER: return "Integer expected";
    case STATUS_ERROR_COMPONENTS_ASSYMETRY: return "Face components assymetry";
    case STATUS_ERROR_UNDEFINED_INDEX: return "Undefined index";
    case STATUS_ERROR_INPUT: return "Error opening file";
    case STATUS_ERROR_INPUT_EMPTY: return "File is empty";
    default: return "Reserved state";
    }
}

Parser::Status Parser::statusType(Parser::Status const status)
{
    bool const isVerbose = (status > STATUS_DEBUG_BEGIN && status < STATUS_DEBUG_END);
    if (isVerbose)
        return STATUS_VERBOSE;

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

void Parser::skipUntilContent(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && (lineIter->isSpace() || *lineIter == '/'))
        ++lineIter;
}

void Parser::skipUntilDelimiter(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && !(lineIter->isSpace() || *lineIter == '/'))
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
    skipWhiteSpace(lineEnd, lineIter);

    if (lineIter >= lineEnd)
        return LINETYPE_EMPTY;

    if (*lineIter == '#')
        return LINETYPE_COMMENT;

    if (*lineIter == 'v') {
        if (isNextCharEndOrSpace(lineEnd, lineIter))
            return LINETYPE_VERTEX_GEOMETRIC;
        if (*lineIter == 't')
            if (isNextCharEndOrSpace(lineEnd, lineIter))
                return LINETYPE_VERTEX_TEXTURE;
        if (*lineIter == 'n')
            if (isNextCharEndOrSpace(lineEnd, lineIter)) {
                return LINETYPE_NORMAL;
            }
    }

    if (*lineIter == 'f')
        if (isNextCharEndOrSpace(lineEnd, lineIter))
            return LINETYPE_FACE;

    return LINETYPE_UNKNOWN;
}


Parser::Status Parser::parseInteger(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        int &outInteger)
{
//    skipWhiteSpace(lineEnd, lineIter);
//    if (isEndOrSpace(lineEnd, lineIter))
//        return STATUS_ERROR_EXPECTED_FLOAT;

    QChar const * const contentBegin = lineIter;
    skipUntilDelimiter(lineEnd, lineIter);
    QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outInteger = content.toInt(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin;
        return STATUS_ERROR_EXPECTED_INTEGER;
    }

    return STATUS_OK;
}

Parser::Status Parser::parseFloat(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        float &outFloat)
{
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_FLOAT;

    QChar const * const contentBegin = lineIter;
    skipUntilDelimiter(lineEnd, lineIter);
    QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outFloat = content.toFloat(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin;
        return STATUS_ERROR_EXPECTED_FLOAT;
    }

    return STATUS_OK;
}

Parser::Status Parser::parseVertexGeometric(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector3D &outVertex)
{
    float parsedValue = 0.0;
    Status status = STATUS_OK;

    for (int i = 0; i < 3; ++i) {
        status = parseFloat(lineEnd, lineIter, parsedValue);
        if (status != STATUS_OK)
            return status;
        outVertex[i] = parsedValue;
    }

    return status;
}

Parser::Status Parser::parseVertexTexture(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector2D &outVertex)
{
    float parsedValue = 0.0;
    Status status = STATUS_OK;

    for (int i = 0; i < 2; ++i) {
        status = parseFloat(lineEnd, lineIter, parsedValue);
        if (status != STATUS_OK)
            return status;
        outVertex[i] = parsedValue;
    }

    return status;
}

Parser::Status Parser::parseFaceVertexComponents(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        int &indexGeometric,
        bool &hasIndexTexture,
        int &indexTexture,
        bool &hasIndexNormal,
        int &indexNormal)
{
    /* All possible cases
     * case #1: 1
     * case #2: 1/
     * case #3: 1//
     * case #4: 1//1
     * case #5: 1/1
     * case #6: 1/1/
     * case #7: 1/1/1
     */
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_INTEGER;

    Status status = STATUS_OK;

    // case #1
    status = parseInteger(lineEnd, lineIter, indexGeometric);
    if (status != STATUS_OK)
        return status;
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = false;
        hasIndexNormal = false;
        return STATUS_OK;
    }

    ++lineIter;

    // case #2
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = false;
        hasIndexNormal = false;
        return STATUS_OK;
    }

    if (*lineIter == '/') {
        ++lineIter; // skip '/'

        // case #3
        if (isEndOrSpace(lineEnd, lineIter)) {
            hasIndexTexture = false;
            hasIndexNormal = false;
            return STATUS_OK;
        }

        // case #4
        status = parseInteger(lineEnd, lineIter, indexNormal);
        if (status != STATUS_OK)
            return status;
        if (isEndOrSpace(lineEnd, lineIter)) {
            hasIndexTexture = false;
            hasIndexNormal = true;
            return STATUS_OK;
        }
    }

    // case #5
    status = parseInteger(lineEnd, lineIter, indexTexture);
    if (status != STATUS_OK)
        return status;
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = true;
        hasIndexNormal = false;
        return STATUS_OK;
    }

    lineIter++; // skip '/'

    // case #6
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = true;
        hasIndexNormal = false;
        return STATUS_OK;
    }

    // case #7
    status = parseInteger(lineEnd, lineIter, indexNormal);
    if (status != STATUS_OK)
        return status;
    if (isEndOrSpace(lineEnd, lineIter)) {
        hasIndexTexture = true;
        hasIndexNormal = true;
        return STATUS_OK;
    }

    return status;
}

Parser::Status Parser::parseFace(
        int numVerticesGeometric,
        int numVerticesTexture,
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<int> &outFaceGeometric,
        QVector<int> &outFaceTexture,
        QVector<int> &outFaceNormal)
{
    int indexGeometric = 0;
    bool mustHaveIndexTexture = false;
    bool hasIndexTexture = false;
    int indexTexture = 0;
    bool mustHaveIndexNormal = false;
    bool hasIndexNormal = false;
    int indexNormal = 0;

    Status status = STATUS_OK;

    status = parseFaceVertexComponents(
                lineEnd, lineIter,
                indexGeometric,
                hasIndexTexture, indexTexture,
                hasIndexNormal, indexNormal);
    if (status != STATUS_OK)
        return status;
    if (indexGeometric < 1 || indexGeometric > numVerticesGeometric)
        return STATUS_ERROR_UNDEFINED_INDEX;
    outFaceGeometric.append(indexGeometric);
    if (hasIndexTexture) {
        mustHaveIndexTexture = true;
        if (indexTexture < 1 || indexTexture > numVerticesTexture)
            return STATUS_ERROR_UNDEFINED_INDEX;
        outFaceTexture.append(indexTexture);
    }
    if (hasIndexNormal) {
        mustHaveIndexNormal= true;
        // TODO check index of normal
        outFaceNormal.append(indexNormal);
    }

    for (int i = 0; i < 2; ++i) {
        status = parseFaceVertexComponents(
                    lineEnd, lineIter,
                    indexGeometric,
                    hasIndexTexture, indexTexture,
                    hasIndexNormal, indexNormal);
        if (status != STATUS_OK)
            return status;
        if (indexGeometric < 1 || indexGeometric > numVerticesGeometric)
            return STATUS_ERROR_UNDEFINED_INDEX;
        outFaceGeometric.append(indexGeometric);
        if (mustHaveIndexTexture != hasIndexTexture
                || mustHaveIndexNormal != hasIndexNormal)
            return STATUS_ERROR_COMPONENTS_ASSYMETRY;
        if (hasIndexTexture) {
            if (indexTexture < 1 || indexTexture > numVerticesTexture)
                return STATUS_ERROR_UNDEFINED_INDEX;
            outFaceTexture.append(indexTexture);
        }
        if (hasIndexNormal)
            outFaceNormal.append(indexNormal);
    }

    while (!isEndOrSpace(lineEnd, lineIter)) {
        status = parseFaceVertexComponents(
                    lineEnd, lineIter,
                    indexGeometric,
                    hasIndexTexture, indexTexture,
                    hasIndexNormal, indexNormal);
       if (status != STATUS_OK)
           return status;
       if (indexGeometric < 1 || indexGeometric > numVerticesGeometric)
           return STATUS_ERROR_UNDEFINED_INDEX;
       outFaceGeometric.append(indexGeometric);

       if (mustHaveIndexTexture != hasIndexTexture
               || mustHaveIndexNormal != hasIndexNormal)
           return STATUS_ERROR_COMPONENTS_ASSYMETRY;
       if (hasIndexTexture) {
           if (indexTexture < 1 || indexTexture > numVerticesTexture)
               return STATUS_ERROR_UNDEFINED_INDEX;
           outFaceTexture.append(indexTexture);
       }
       if (hasIndexNormal)
           outFaceNormal.append(indexNormal);
    }

    return status;
}

Parser::ParserState Parser::load(QTextStream &input, Mesh &outMesh)
{
    ParserState parserState;
    outMesh.clear();

    while (!input.atEnd()) {
        const QString line = input.readLine();
        ++parserState.lineNumber;

        if (line.isEmpty())
            continue;

        // TODO multiline statements support (when the line ends with '\')

        QChar const *lineIter = line.begin();
        QChar const * const lineEnd = line.end();

        const LineType lineType = parseLineType(lineEnd, lineIter);

        switch (lineType) {
        case LINETYPE_EMPTY:
            continue;
        case LINETYPE_COMMENT:
            continue;

        case LINETYPE_VERTEX_GEOMETRIC: {
            QVector3D vertexGeometric;
            parserState.status = parseVertexGeometric(lineEnd, lineIter, vertexGeometric);
            if (parserState.status != STATUS_OK) {
                parserState.columnNumber = lineIter - line.begin();
                return parserState;
            }
            outMesh.geometryVertices.append(vertexGeometric);
            // qDebug() << vertexGeometric;
            break;
        }

        case LINETYPE_VERTEX_TEXTURE: {
            QVector2D vertexTexture;
            parserState.status = parseVertexTexture(lineEnd, lineIter, vertexTexture);
            if (parserState.status != STATUS_OK) {
                parserState.columnNumber = lineIter - line.begin();
                return parserState;
            }
            outMesh.textureVertices.append(vertexTexture);
//            qDebug() << vertexTexture;
            break;
        }

        case LINETYPE_NORMAL: {
            QVector3D normal;
            parserState.status = parseVertexGeometric(lineEnd, lineIter, normal);
            if (parserState.status != STATUS_OK) {
                parserState.columnNumber = lineIter - line.begin();
                return parserState;
            }
//            qDebug() << normal;
            break;
        }

        case LINETYPE_FACE: {
            QVector<int> faceGeometric;
            QVector<int> faceTexture;
            QVector<int> faceNormal;
            parserState.status = parseFace(
                    outMesh.geometryVertices.length(), // is not used for now
                    outMesh.textureVertices.length(), // is not used for now
                    lineEnd, lineIter,
                    faceGeometric, faceTexture, faceNormal);
            if (parserState.status != STATUS_OK) {
                parserState.columnNumber = lineIter - line.begin();
                return parserState;
            }
            outMesh.geometryFaces.append(faceGeometric);
            outMesh.textureFaces.append(faceTexture);
//            qDebug() << faceGeometric << faceTexture << faceNormal;
            break;
        }

        case LINETYPE_UNKNOWN:
            break;

        default:
            return STATUS_ERROR_STATE_BROKEN;
        }
    }

    if (outMesh.geometryVertices.length() == 0
            && outMesh.textureVertices.length() == 0
            && outMesh.geometryFaces.length() == 0
            && outMesh.textureFaces.length() == 0)
        parserState.status = STATUS_ERROR_INPUT_EMPTY;

    return parserState;
}


Parser::ParserState::ParserState(Status status):
        lineNumber(0), columnNumber(0), status(status)
{}

Parser::ParserState::ParserState():
        ParserState(STATUS_OK)
{}

Parser::ParserState Parser::load(QTextStream &&input, Mesh &outMesh)
{
    return load(input, outMesh);
}

Parser::ParserState Parser::load(QString const &filename, Mesh &outMesh)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ParserState(STATUS_ERROR_INPUT);
    QTextStream input(&file);

    return load(input, outMesh);
}

} // namespace Newpson::Parser::OBJ
