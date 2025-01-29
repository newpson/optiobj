#include "Mesh.hpp"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson {

char const * Mesh::statusToString(Mesh::Status status)
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

bool Mesh::isComment(QString &line)
{
    return line[0] == CHAR_COMMENT;
}

Mesh::Status Mesh::statusType(Mesh::Status status)
{
    if (status > STATUS_VERBOSE_BEGIN && status < STATUS_VERBOSE_END)
        return STATUS_VERBOSE;
    else if (status > STATUS_WARN_BEGIN && status < STATUS_WARN_END)
        return STATUS_WARN;
    else if (status > STATUS_ERROR_BEGIN && status < STATUS_ERROR_END)
        return STATUS_ERROR;
    return STATUS_OK;
}

Mesh::Mesh() {}

Mesh::Status Mesh::parseGeometryVertex(QVector3D &outVertex, QStringList &tokens)
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

Mesh::Status Mesh::parseTextureVertex(QVector2D &outVertex, QStringList &tokens)
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

Mesh::Status Mesh::parseFace(
    int numGeometryVertices,
    int numTextureVertices,
    QVector<int> &outGeometryFace,
    QVector<int> &outTextureFace,
    QStringList &tokens)
{
    if (tokens.length() < LENGTH_MIN_DATATYPE_FACE)
        return STATUS_ERROR_INVALID_PARAMETERS_FACE;

    for (auto triadIter = tokens.begin() + 1; triadIter != tokens.end(); ++triadIter) {
        QString &triad = *triadIter;

        QStringList components = triad.split(CHAR_DELIMITER);
        if (components.length() < LENGTH_MIN_VERTEX_COMPONENTS)
            return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;

        bool parseSuccess;
        int indexGeometry, indexTexture;
        if (!components[INDEX_TOKEN_VERTEX_COMPONENT_GEOMETRY].isEmpty()) {
            indexGeometry = components[INDEX_TOKEN_VERTEX_COMPONENT_GEOMETRY].toUInt(&parseSuccess);
            if (!parseSuccess)
                return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
            if (indexGeometry > numGeometryVertices)
                return STATUS_ERROR_INVALID_INDEX_VERTEX_GEOMETRY;
            outGeometryFace.append(indexGeometry);
        } else {
            return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
        }

        // #3: throw error if there is no texture component on some of the vertices
        if (components.length() > LENGTH_MIN_VERTEX_COMPONENTS) {
            if (!components[INDEX_TOKEN_VERTEX_COMPONENT_TEXTURE].isEmpty()) {
                indexTexture = components[INDEX_TOKEN_VERTEX_COMPONENT_TEXTURE].toUInt(&parseSuccess);
                if (!parseSuccess)
                    return STATUS_ERROR_INVALID_PARAMETERS_COMPONENTS;
                if (indexTexture > numTextureVertices)
                    return STATUS_ERROR_INVALID_INDEX_VERTEX_TEXTURE;
                outTextureFace.append(indexTexture);
            }
        }
    }

    return STATUS_OK;
}

Mesh::Status Mesh::evaluateTokens(Mesh &outMesh, QStringList &tokens)
{
    Mesh::Status status = STATUS_OK;
    QString &datatype = tokens[Mesh::INDEX_TOKEN_DATATYPE];

    // TODO may be faster variant exists?
    if (datatype[0] == 'v') {
        if (datatype.length() > 1) {
            if (datatype[1] == 't') {
                QVector2D vertex;
                status = parseTextureVertex(vertex, tokens);
                if (status <= STATUS_ERROR) {
                    outMesh.textureVertices.append(vertex);
                    qDebug() << vertex;
                }
            }
        } else {
            QVector3D vertex;
            status = parseGeometryVertex(vertex, tokens);
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
            geometryFace,
            textureFace,
            tokens);
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

Mesh::Status Mesh::read(Mesh &outMesh, QTextStream &input)
{
    while (!input.atEnd()) {
        QString line = input.readLine();
        if (line.isEmpty() || Mesh::isComment(line))
            continue;

        QStringList tokens = line.split(CHAR_SPACE);

        Mesh::Status status = evaluateTokens(outMesh, tokens);
        // #2: like here
        if (Mesh::statusType(status) == STATUS_WARN)
            qDebug() << Mesh::statusToString(status);
        // #2:
        else if (status >= STATUS_ERROR)
            return status;
    }
    return STATUS_OK;
}

Mesh::Status Mesh::read(Mesh &outMesh, QTextStream &&input)
{
    return Mesh::read(outMesh, input);
}


Mesh::Status Mesh::read(Mesh &outMesh, QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return STATUS_ERROR_IO;

    QTextStream input(&file);
    if (input.atEnd())
        return STATUS_WARN_IO_EMPTY;

    return read(outMesh, input);
}

Mesh::Status Mesh::read(Mesh &outMesh, QString &&filename)
{
    return read(outMesh, filename);
}

Mesh::Status Mesh::read(Mesh &outMesh, char const *filename)
{
    return read(outMesh, QString(filename));
}

} // namespace Newpson
