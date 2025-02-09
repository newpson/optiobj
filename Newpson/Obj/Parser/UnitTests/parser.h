#ifndef NEWPSON_OBJ_PARSER_UNITTESTS_PARSER_H
#define NEWPSON_OBJ_PARSER_UNITTESTS_PARSER_H

#include <QObject>
#include <QString>
#include <QTest>
#include "Newpson/Mesh/mesh.h"
#include "Newpson/Obj/Parser/parser.h"

Q_DECLARE_METATYPE(Newpson::ObjParser::Status)
Q_DECLARE_METATYPE(Newpson::Mesh::ValidationResult)

namespace Newpson::ObjParser::Test {

class Parser: public QObject
{
    Q_OBJECT

public:
    Parser(QObject *parent = nullptr);

private slots:
    void testIndexMakeAbsolute_data();
    void testIndexMakeAbsolute();

    void testSkipWhiteSpace_data();
    void testSkipWhiteSpace();

    void testParseFloat_data();
    void testParseFloat();

    void testNumArguments_data();
    void testNumArguments();

    void testEmptyFile_data();
    void testEmptyFile();

    void testComponentsCoherence_data();
    void testComponentsCoherence();

    void testUndefinedIndex_data();
    void testUndefinedIndex();

    void testParsedData();

private:
    Newpson::ObjParser::ParserResult m_parserResult;
    QString m_commonPart;
};

}

#endif
