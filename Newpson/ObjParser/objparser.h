#ifndef NEWPSON_OBJPARSER_PARSER_H
#define NEWPSON_OBJPARSER_PARSER_H

#include "Newpson/Mesh/mesh.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

namespace Newpson::ObjParser {

enum Status {
    STATUS_VERBOSE,
    STATUS_DEBUG_BEGIN,
    STATUS_OK,
    STATUS_DEBUG_END,

    STATUS_ERROR,
    STATUS_ERROR_BEGIN,
    STATUS_ERROR_STATE_BROKEN,
    STATUS_ERROR_EXPECTED_FLOAT,
    STATUS_ERROR_EXPECTED_INTEGER,
    STATUS_ERROR_EXPECTED_STRING,
    STATUS_ERROR_COMPONENTS_INCOHERENCE,
    STATUS_ERROR_UNDEFINED_INDEX,
    STATUS_ERROR_INPUT,
    STATUS_ERROR_INPUT_EMPTY,
    STATUS_ERROR_END,

    STATUS_RESERVED,
};

struct ParserResult
{
    Status status;
    int lineNumber;
    int columnNumber;

    ParserResult();
    ParserResult(Status status);
};

QString statusToString(Status status);
Status statusType(Status const status);

Mesh load(QTextStream &input, ParserResult &parserResult);
Mesh load(QTextStream &&input, ParserResult &parserResult);
Mesh load(QString const &filename, ParserResult &parserResult);

}

#endif // NEWPSON_OBJPARSER_PARSER_H
