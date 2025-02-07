#include "ObjParser/objparser.h"
#include "ObjParser/objparserinternal.h"

#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QDebug>

#include "Newpson/Mesh/mesh.h"

namespace Newpson::ObjParser {

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

void skipWhiteSpace(QChar const * const lineEnd, QChar const *&lineIter)
{
    while (lineIter < lineEnd && lineIter->isSpace())
        ++lineIter;
}

void skipUntilSlashOrSpace(QChar const * const lineEnd, QChar const *&lineIter)
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
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_FLOAT;

    const QChar * const contentBegin = lineIter;
    skipUntilSlashOrSpace(lineEnd, lineIter);
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

int indexMakeAbsolute(const int index, const int numComponents)
{
    return (index >= 0 ? index - 1 : numComponents + index);
}

Status appendIndex(const int index, QVector<int> &indices, const int numComponents)
{
    const int absIndex = indexMakeAbsolute(index, numComponents);
    if (absIndex < 0 || absIndex >= numComponents)
        return STATUS_ERROR_UNDEFINED_INDEX;
    indices.append(absIndex);
    return STATUS_OK;
}

Status parseFaceTriad(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        const int numVertices,
        const int numVerticesTexture,
        const int numNormals,
        QVector<int> &indicesVertices,
        QVector<int> &indicesVerticesTexture,
        QVector<int> &indicesNormals,
        bool &hasIndexVertexTexture,
        bool &hasIndexNormal)
{
    Status status = STATUS_OK;

    int indexVertex = 0;
    int indexVertexTexture = 0;
    int indexNormal = 0;
    status = parseFaceVertexComponents(
                lineEnd, lineIter,
                indexVertex,
                hasIndexVertexTexture, indexVertexTexture,
                hasIndexNormal, indexNormal);
    if (status != STATUS_OK)
        return status;

    status = appendIndex(indexVertex, indicesVertices, numVertices);
    if (status != STATUS_OK)
        return status;

    if (hasIndexVertexTexture) {
        status = appendIndex(indexVertexTexture, indicesVerticesTexture, numVerticesTexture);
        if (status != STATUS_OK)
            return status;
    }

    if (hasIndexNormal) {
        status = appendIndex(indexNormal, indicesNormals, numNormals);
        if (status != STATUS_OK)
            return status;
    }

    return STATUS_OK;
}


Status parseFace(
        const int numVertices,
        const int numVerticesTexture,
        const int numNormals,
        QChar const * const lineEnd,
        QChar const *&lineIter,
        int &numParsedTriads,
        QVector<int> &indicesVertices,
        QVector<int> &indicesVerticesTexture,
        QVector<int> &indicesNormals)
{
    Status status = STATUS_OK;
    numParsedTriads = 0;

    bool hasIndexVertexTexture = false;
    bool hasIndexNormal = false;
    status = parseFaceTriad(
                lineEnd, lineIter,
                numVertices, numVerticesTexture, numNormals,
                indicesVertices, indicesVerticesTexture, indicesNormals,
                hasIndexVertexTexture, hasIndexNormal);
    if (status != STATUS_OK)
        return status;
    ++numParsedTriads;

    bool mustHaveIndexVertexTexture = false;
    bool mustHaveIndexNormal = false;
    if (hasIndexVertexTexture)
        mustHaveIndexVertexTexture = true;
    if (hasIndexNormal)
        mustHaveIndexNormal = true;

    while (lineIter != lineEnd) {
        status = parseFaceTriad(
                    lineEnd, lineIter,
                    numVertices, numVerticesTexture, numNormals,
                    indicesVertices, indicesVerticesTexture, indicesNormals,
                    hasIndexVertexTexture, hasIndexNormal);

        if (status != STATUS_OK)
            return status;

        if (mustHaveIndexVertexTexture != hasIndexVertexTexture
                || mustHaveIndexNormal != hasIndexNormal)
            return STATUS_ERROR_COMPONENTS_INCOHERENCE;

        ++numParsedTriads;
        skipWhiteSpace(lineEnd, lineIter);
    }

    if (numParsedTriads < 3)
        return STATUS_ERROR_EXPECTED_INTEGER;

    return STATUS_OK;
}

Status parseGroupName(
        QChar const * const lineEnd,
        QChar const *&lineIter,
        QVector<QString> &groupsNames)
{
    skipWhiteSpace(lineEnd, lineIter);
    if (isEndOrSpace(lineEnd, lineIter))
        return STATUS_ERROR_EXPECTED_STRING;

    // parse first group name, discard other
    const QChar * const contentBegin = lineIter;
    skipUntilWhiteSpace(lineEnd, lineIter);
    groupsNames.append(QStringView(contentBegin, lineIter).toString());

    return STATUS_OK;
}

QVector3D generateNormal(
        const QVector<QVector3D> &vertices,
        const QVector<int> &indicesVertices,
        const int faceBegin,
        const int faceEnd)
{
    Q_ASSERT(faceBegin >= 0 && faceBegin <= indicesVertices.length());
    Q_ASSERT(faceEnd >= 0 && faceEnd <= indicesVertices.length());

    const int numNormals = faceEnd - faceBegin - 2;
    QVector3D sumNormals;
    for (int vi = faceBegin + 1; vi < faceEnd-1; ++vi) {
        const QVector3D v1(vertices[indicesVertices[vi]] - vertices[indicesVertices[faceBegin]]);
        const QVector3D v2(vertices[indicesVertices[vi+1]] - vertices[indicesVertices[faceBegin]]);
        sumNormals += QVector3D::normal(v1, v2);
    }

    return QVector3D(sumNormals.x()/numNormals, sumNormals.y()/numNormals, sumNormals.z()/numNormals);
}

QVector2D generateVerticesTexture()
{
    return QVector2D(0.0, 0.0);
}

}

Mesh load(QTextStream &input, ParserResult &parserResult)
{
    using namespace Internal;

    QVector<QVector3D> vertices;
    QVector<QVector2D> verticesTexture;
    QVector<QVector3D> normals;
    QVector<int> indicesVertices;
    QVector<int> indicesVerticesTexture;
    QVector<int> indicesNormals;
    QVector<int> facesEnds;
    QVector<QString> groupsNames = {"default"};
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
            int numParsedTriads = 0;
            parserResult.status = parseFace(
                    vertices.length(), verticesTexture.length(), normals.length(),
                    lineEnd, lineIter, numParsedTriads,
                    indicesVertices, indicesVerticesTexture, indicesNormals);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }

            bool hasVerticesTexture = (indicesVerticesTexture.length() == indicesVertices.length());
            if (!hasVerticesTexture) {
                verticesTexture.append(QVector2D(0.0, 0.0));
                for (int i = 0; i < numParsedTriads; ++i)
                    indicesVerticesTexture.append(verticesTexture.length() - 1);
            }

            bool hasNormals = (indicesNormals.length() == indicesVertices.length());
            if (!hasNormals) {
                QVector3D absNormal = generateNormal(vertices,
                                                     indicesVertices,
                                                     facesEnds.length() > 0 ? facesEnds.last() : 0,
                                                     indicesVertices.length());
                normals.append(absNormal);
                for (int i = 0; i < numParsedTriads; ++i)
                    indicesNormals.append(normals.length() - 1);
            }

            facesEnds.append(indicesVertices.length());
            break;
        }

        case LINETYPE_GROUP: {
            parserResult.status = parseGroupName(lineEnd, lineIter, groupsNames);
            if (parserResult.status != STATUS_OK) {
                parserResult.columnNumber = lineIter - line.begin();
                return Mesh();
            }
            groupsEnds.append(facesEnds.length());
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

    groupsEnds.append(facesEnds.length());

    return Mesh(
        vertices,
        verticesTexture,
        normals,
        indicesVertices,
        indicesVerticesTexture,
        indicesNormals,
        facesEnds,
        groupsNames,
        groupsEnds);
}

ParserResult::ParserResult(Status status):
        status(status), lineNumber(0), columnNumber(0)
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

}
