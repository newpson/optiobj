#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QDebug>

#include "mesh.h"
#include "parser.h"
#include "parser-internal.h"

namespace Newpson::Parsing::Obj {

QString statusToString(Status const status)
{
    switch (status) {
    case STATUS_OK: return "OK";
    case STATUS_ERROR_STATE_BROKEN: return "State broken";
    case STATUS_ERROR_EXPECTED_FLOAT: return "Float expected";
    case STATUS_ERROR_EXPECTED_INTEGER: return "Integer expected";
    case STATUS_ERROR_COMPONENTS_INCOHERENCE: return "Face components assymetry";
    case STATUS_ERROR_UNDEFINED_INDEX: return "Undefined index";
    case STATUS_ERROR_INPUT: return "Error opening file";
    case STATUS_ERROR_INPUT_EMPTY: return "File is empty";
    case STATUS_ERROR_EXPECTED_STRING: return "String expected";
    default: return "Reserved state";
    }
}

Status statusType(Status const status)
{
    bool const isVerbose = (status > STATUS_DEBUG_BEGIN && status < STATUS_DEBUG_END);
    if (isVerbose)
        return STATUS_VERBOSE;

    bool const isError = (status > STATUS_ERROR_BEGIN && status < STATUS_ERROR_END);
    if (isError)
        return STATUS_ERROR;

    return STATUS_RESERVED;
}

namespace Internal {

bool skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    QChar const *begin = lineIter;
    while (lineIter < lineEnd && lineIter->isSpace())
        ++lineIter;
    bool hasSkipped = lineIter > begin;
    return hasSkipped;
}

void skipUntilContent(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && (lineIter->isSpace() || *lineIter == '/'))
        ++lineIter;
}


void skipUntilDelimiter(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && !(lineIter->isSpace() || *lineIter == '/'))
        ++lineIter;
}

bool isEndOrSpace(QChar const * const lineEnd, QChar const * const lineIter)
{
    return (lineIter >= lineEnd || lineIter->isSpace());
}

void skipUntilWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (!isEndOrSpace(lineEnd, lineIter))
        ++lineIter;
}

bool isNextCharEndOrSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    ++lineIter;
    return isEndOrSpace(lineEnd, lineIter);
}

LineType parseLineType(QChar const * const lineEnd, QChar const *&lineIter)
{
    skipWhiteSpace(lineEnd, lineIter);

    if (lineIter >= lineEnd)
        return LINETYPE_EMPTY;

    if (*lineIter == '#')
        return LINETYPE_COMMENT;

    if (*lineIter == 'v') {
        if (isNextCharEndOrSpace(lineEnd, lineIter))
            return LINETYPE_VERTEX;
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

    if (*lineIter == 'g')
        if (isNextCharEndOrSpace(lineEnd, lineIter))
            return LINETYPE_GROUP;

    return LINETYPE_UNKNOWN;
}


Status parseInteger(
        const QChar * const lineEnd,
        const QChar *&lineIter,
        int &outInteger)
{
//    skipWhiteSpace(lineEnd, lineIter);
//    if (isEndOrSpace(lineEnd, lineIter))
//        return STATUS_ERROR_EXPECTED_FLOAT;

    const QChar * const contentBegin = lineIter;
    skipUntilDelimiter(lineEnd, lineIter);
    const QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outInteger = content.toInt(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin;
        return STATUS_ERROR_EXPECTED_INTEGER;
    }

    return STATUS_OK;
}

Status parseFloat(
        const QChar * const lineEnd,
        const QChar *&lineIter,
        float &outFloat)
{
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_FLOAT;

    const QChar * const contentBegin = lineIter;
    skipUntilWhiteSpace(lineEnd, lineIter);
    const QStringView content(contentBegin, lineIter);

    bool isParseSuccessful = false;
    outFloat = content.toFloat(&isParseSuccessful);
    if (!isParseSuccessful) {
        lineIter = contentBegin;
        return STATUS_ERROR_EXPECTED_FLOAT;
    }

    return STATUS_OK;
}

Status parseVertexGeometric(
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

Status parseVertexTexture(
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

Status parseFaceVertexComponents(
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
     * case #2: 1/ (illegal)
     * case #3: 1// (illegal)
     * case #4: 1//1
     * case #5: 1/1
     * case #6: 1/1/ (illegal)
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

Status parseFace(
        const int numVertices,
        const int numVerticesTexture,
        const int numNormals,
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<int> &outFacesVertices,
        QVector<int> &outFacesVerticesTexture,
        QVector<int> &outFacesNormals)
{
    int indexVertex = 0;
    bool mustHaveIndexTexture = false;
    bool hasIndexTexture = false;
    int indexVertexTexture = 0;
    bool mustHaveIndexNormal = false;
    bool hasIndexNormal = false;
    int indexNormal = 0;

    Status status = STATUS_OK;

    status = parseFaceVertexComponents(
                lineEnd, lineIter,
                indexVertex,
                hasIndexTexture, indexVertexTexture,
                hasIndexNormal, indexNormal);
    if (status != STATUS_OK)
        return status;
    if (indexVertex == 0 || indexVertex > numVertices)
        return STATUS_ERROR_UNDEFINED_INDEX;
    outFacesVertices.append(indexVertex - 1);
    if (hasIndexTexture) {
        mustHaveIndexTexture = true;
        if (indexVertexTexture == 0 || indexVertexTexture > numVerticesTexture)
            return STATUS_ERROR_UNDEFINED_INDEX;
        outFacesVerticesTexture.append(indexVertexTexture - 1);
    }
    if (hasIndexNormal) {
        mustHaveIndexNormal= true;
        if (indexNormal == 0 || indexNormal > numNormals)
             return STATUS_ERROR_UNDEFINED_INDEX;
        outFacesNormals.append(indexNormal - 1);
    }

    for (int i = 0; i < 2; ++i) {
        status = parseFaceVertexComponents(
                    lineEnd, lineIter,
                    indexVertex,
                    hasIndexTexture, indexVertexTexture,
                    hasIndexNormal, indexNormal);
        if (status != STATUS_OK)
            return status;
        if (indexVertex == 0 || indexVertex > numVertices)
            return STATUS_ERROR_UNDEFINED_INDEX;
        outFacesVertices.append(indexVertex - 1);
        if (mustHaveIndexTexture != hasIndexTexture
                || mustHaveIndexNormal != hasIndexNormal)
            return STATUS_ERROR_COMPONENTS_INCOHERENCE;
        if (hasIndexTexture) {
            if (indexVertexTexture == 0 || indexVertexTexture > numVerticesTexture)
                return STATUS_ERROR_UNDEFINED_INDEX;
            outFacesVerticesTexture.append(indexVertexTexture - 1);
        }
        if (hasIndexNormal) {
            if (indexNormal == 0 || indexNormal > numNormals)
                 return STATUS_ERROR_UNDEFINED_INDEX;
            outFacesNormals.append(indexNormal - 1);
        }
    }

    while (lineIter != lineEnd) {
        skipWhiteSpace(lineEnd, lineIter);
        if (lineIter == lineEnd)
            return STATUS_OK;
        status = parseFaceVertexComponents(
                    lineEnd, lineIter,
                    indexVertex,
                    hasIndexTexture, indexVertexTexture,
                    hasIndexNormal, indexNormal);
       if (status != STATUS_OK)
           return status;
       if (indexVertex == 0 || indexVertex > numVertices)
           return STATUS_ERROR_UNDEFINED_INDEX;
       outFacesVertices.append(indexVertex - 1);

       if (mustHaveIndexTexture != hasIndexTexture
               || mustHaveIndexNormal != hasIndexNormal)
           return STATUS_ERROR_COMPONENTS_INCOHERENCE;
       if (hasIndexTexture) {
           if (indexVertexTexture == 0 || indexVertexTexture > numVerticesTexture)
               return STATUS_ERROR_UNDEFINED_INDEX;
           outFacesVerticesTexture.append(indexVertexTexture - 1);
       }

       if (hasIndexNormal) {
           if (indexNormal == 0 || indexNormal > numNormals)
                return STATUS_ERROR_UNDEFINED_INDEX;
           outFacesNormals.append(indexNormal - 1);
       }
    }

    return status;
}

Status parseGroupsNames(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<QString> &groupsNames)
{
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_STRING;

    while (lineIter < lineEnd) {
        const QChar * const contentBegin = lineIter;
        while (!isEndOrSpace(lineEnd, lineIter))
            ++lineIter;
        groupsNames.append(QStringView(contentBegin, lineIter).toString());
        skipWhiteSpace(lineEnd, lineIter);
    }
    return STATUS_OK;
}

void fillRemainingGroupsEnds(
    const int numFaces,
    const int numActiveGroups,
    QVector<int> &groupsEnds)
{
    for (int i = 0; i < numActiveGroups; ++i)
        groupsEnds.append(numFaces);
}

void switchActiveGroups(
    const int numFaces,
    const int numNextActiveGroups,
    QVector<int> &groupsBegins,
    QVector<int> &groupsEnds)
{
    const int numActiveGroups = groupsBegins.length() - groupsEnds.length();
    fillRemainingGroupsEnds(numFaces, numActiveGroups, groupsEnds);
    for (int i = 0; i < numNextActiveGroups; ++i)
        groupsBegins.append(numFaces);
}

} // namespace Internal

Mesh load(QTextStream &input, ParserResult &parserResult)
{
    using namespace Internal;

    QVector<QVector3D> vertices;
    QVector<QVector2D> verticesTexture;
    QVector<QVector3D> normals;
    QVector<int> indicesVertices;
    QVector<int> indicesVerticesTexture;
    QVector<int> indicesNormals;
    QVector<int> facesVertices;
    QVector<int> facesVerticesTexture;
    QVector<int> facesNormals;
    QVector<QString> groupsNames = {"default"};
    QVector<int> groupsBegins = {0};
    QVector<int> groupsEnds;

    parserResult = ParserResult(STATUS_OK);

    while (!input.atEnd()) {
        QString lineBuffer = input.readLine();
        ++parserResult.lineNumber;

        if (lineBuffer.isEmpty())
            continue;

        while (lineBuffer[lineBuffer.length() - 1] == '\\') {
            lineBuffer[lineBuffer.length() - 1] = ' ';
            lineBuffer.append(input.readLine());
            ++parserResult.lineNumber;
        }

        // because of 'mixing const and nonconst iterators' clang warn
        const QString line(lineBuffer);

        QChar const *lineIter = line.begin();
        QChar const * const lineEnd = line.end();

        const LineType lineType = parseLineType(lineEnd, lineIter);

        switch (lineType) {
        case LINETYPE_EMPTY:
            continue;
        case LINETYPE_COMMENT:
            continue;

        case LINETYPE_VERTEX: {
            QVector3D vertex;
            parserResult.status = parseVertexGeometric(lineEnd, lineIter, vertex);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }
            vertices.append(vertex);
            break;
        }

        case LINETYPE_VERTEX_TEXTURE: {
            QVector2D vertex;
            parserResult.status = parseVertexTexture(lineEnd, lineIter, vertex);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }
            verticesTexture.append(vertex);
            break;
        }

        case LINETYPE_NORMAL: {
            QVector3D normal;
            parserResult.status = parseVertexGeometric(lineEnd, lineIter, normal);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }
            normals.append(normal);
            break;
        }

        case LINETYPE_FACE: {
            parserResult.status = parseFace(
                    vertices.length(),
                    verticesTexture.length(),
                    normals.length(),
                    lineEnd, lineIter,
                    indicesVertices, indicesVerticesTexture, indicesNormals);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }

            facesVertices.append(indicesVertices.length());
            facesVerticesTexture.append(indicesVerticesTexture.length());
            facesNormals.append(indicesNormals.length());

            break;
        }

        case LINETYPE_GROUP: {
            const int numGroupsBeforeParse = groupsNames.length();
            parserResult.status = parseGroupsNames(lineEnd, lineIter, groupsNames);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }
            const int numGroupsParsed = groupsNames.length() - numGroupsBeforeParse;
            switchActiveGroups(facesVertices.length(), numGroupsParsed, groupsBegins, groupsEnds);
            break;
        }

        case LINETYPE_UNKNOWN:
            break;

        default:
            parserResult.status = STATUS_ERROR_INPUT_EMPTY;
            return Mesh();
        }
    }

    if (vertices.length() == 0
        && verticesTexture.length() == 0
        && normals.length() == 0)
        parserResult.status = STATUS_ERROR_INPUT_EMPTY;

    fillRemainingGroupsEnds(
        facesVertices.length(),
        groupsBegins.length() - groupsEnds.length(),
        groupsEnds);

    return Mesh(
        vertices,
        verticesTexture,
        normals,
        indicesVertices,
        indicesVerticesTexture,
        indicesNormals,
        facesVertices,
        facesVerticesTexture,
        facesNormals,
        groupsNames,
        groupsBegins,
        groupsEnds);
}

ParserResult::ParserResult(Status status):
        lineNumber(0), columnNumber(0), status(status)
{}

ParserResult::ParserResult():
        ParserResult(STATUS_OK)
{}

Mesh load(QTextStream &&input, ParserResult &parserResult)
{
    return load(input, parserResult);
}

Mesh load(QString const &filename, ParserResult &parserResult)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        parserResult.status = STATUS_ERROR_INPUT;
        return Mesh();
    }

    QTextStream input(&file);
    return load(input, parserResult);
}

} // namespace Newpson::Parser::OBJ
