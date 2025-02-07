#ifndef NEWPSON_OBJPARSER_PARSER_TEST_H
#define NEWPSON_OBJPARSER_PARSER_TEST_H

#include <QObject>
#include <QString>
#include <QTest>
#include "Newpson/Mesh/mesh.h"
#include "ObjParser/objparser.h"

Q_DECLARE_METATYPE(Newpson::ObjParser::Status)
Q_DECLARE_METATYPE(Newpson::Mesh::ValidationResult)

namespace Newpson::ObjParser::Test {

//! REVIEW: добавить постфикс Test
//! REVIEW: длинное имя файла
class NewpsonObjParserTest: public QObject
{
    Q_OBJECT

    //! REVIEW: private, move down
    Newpson::ObjParser::ParserResult parserResult;
    QString commonPart;

public:
    NewpsonObjParserTest(QObject *parent = nullptr);

private slots:
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

};

} // namespace Newpson::Parsing::Obj::Test

#endif // NEWPSON_OBJPARSER_PARSER_TEST_H
