#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include "Newpson/mesh.h"
#include "Obj/parser.h"
#include "Obj/parser-internal.h"
#include "ParserUnitTests/parser.h"

namespace Newpson::Parsing::Obj::Test {

Parser::Parser(QObject *parent):
    QObject(parent),
    commonPart(
        "v   0  0  0 \n" \
        "v   0  0  1 \n" \
        "v   0  1  0 \n" \
        "v   0  1  1 \n" \
        "vt  0  0    \n" \
        "vt  0  1    \n" \
        "vt  1  0    \n" \
        "vt  1  1    \n" \
        "vn  0  0  1 \n" \
        "vn  0  1  0 \n" \
        "vn  1  0  0 \n" \
        "vn  0  0 -1 \n" \
        "vn  0 -1  0 \n" \
        "vn -1  0  0 \n")
{}

void Parser::testSkipWhiteSpace_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("remainder");
    QTest::newRow("empty") << "" << "";
    QTest::newRow("space") << "\u0020" << "";
    QTest::newRow("spaces") << "\u0020\u0020\u0020\u0020\u0020\u0020\u0020" << "";
    QTest::newRow("tab") << "\u000B" << "";
    QTest::newRow("tabs") << "\u000B\u000B\u000B\u000B\u000B\u000B\u000B" << "";
    QTest::newRow("spaces+tabs") << "\u000B\u0020\u000B\u000B\u0020\u000B\u000B\u000B\u000B\u0020" << "";
    QTest::newRow("spaces+tabs") << "\u000B\u0020\u000B\u000B\u0020\u000B\u000B\u000B\u000B\u0020" << "";

    QTest::newRow("content") << "a" << "a";
    QTest::newRow("space+content") << " a" << "a";
    QTest::newRow("spaces+content") << "    a" << "a";
    QTest::newRow("content+space") << "a " << "a ";
    QTest::newRow("content+spaces") << "a    " << "a    ";
}

void Parser::testSkipWhiteSpace()
{
    QFETCH(const QString, input);
    QFETCH(const QString, remainder);

    QString output;
    QChar const *lineIter = input.begin();
    QChar const * const lineEnd = input.end();
    Newpson::Parsing::Obj::Internal::skipWhiteSpace(lineEnd, lineIter);
    output.append(QStringView(lineIter, lineEnd));

    QCOMPARE(output, remainder);
}

void Parser::testParseFloat_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<Newpson::Parsing::Obj::Status>("status");
    QTest::newRow("empty") << "" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("integer+trash-ascii") << "123abc" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("integer+trash-unicode") << "123\u1234\u1232\u0987" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("trash-ascii+integer") << "abc123" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("trash-unicode+integer") << "\u1234\u1232\u0987123" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("integer+slash") << "123/" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("integer+doubleslash") << "123//" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("slash+integer") << "/123" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("doubleslash+integer") << "//123" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("integer") << "123" << STATUS_OK;
    QTest::newRow("float") << "123.000" << STATUS_OK;
    QTest::newRow("float-nofrac") << "123." << STATUS_OK;
    QTest::newRow("float-noint") << ".000" << STATUS_OK;
    QTest::newRow("float-positive") << "+123.0" << STATUS_OK;
    QTest::newRow("float-negative") << "-123.0" << STATUS_OK;
    QTest::newRow("float-enot") << "123e5" << STATUS_OK;
    QTest::newRow("float-enot-0") << "123e05" << STATUS_OK;
    QTest::newRow("float-enot-plus") << "123e+5" << STATUS_OK;
    QTest::newRow("float-enot-minus") << "123e-5" << STATUS_OK;
    QTest::newRow("wspace-left") << " 123.0" << STATUS_OK;
    QTest::newRow("wspaces-left") << "    123.0" << STATUS_OK;
    QTest::newRow("wspaces-right") << "123.0    " << STATUS_OK;
    QTest::newRow("wspaces-left+right") << "    123.0    " << STATUS_OK;
}

void Parser::testParseFloat()
{
    QFETCH(const QString, input);
    QFETCH(Newpson::Parsing::Obj::Status, status);

    float parsedValue;
    QChar const *lineIter = input.begin();
    QChar const * const lineEnd = input.end();
    parserResult.status = Newpson::Parsing::Obj::Internal::parseFloat(lineEnd, lineIter, parsedValue);
    qDebug() << parserResult.status;
    QCOMPARE(parserResult.status, status);
}

void Parser::testNumArguments_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<Newpson::Parsing::Obj::Status>("status");
    QTest::newRow("vertex-1") << "v 1" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("vertex-2") << "v 1 1" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("vertexTexture-1") << "vt 1" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("normal-1") << "vn 1" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("normal-2") << "vn 1 1" << STATUS_ERROR_EXPECTED_FLOAT;
    QTest::newRow("face-1") << "v 1 1 1 \n f 1" << STATUS_ERROR_EXPECTED_INTEGER;
    QTest::newRow("face-2") << "v 1 1 1 \n f 1 1" << STATUS_ERROR_EXPECTED_INTEGER;

    QTest::newRow("vertex-3") << "v 1 1 1" << STATUS_OK;
    QTest::newRow("vertex-many") << "v 1 1 1 1 1 1 1" << STATUS_OK;
    QTest::newRow("vertexTexture-3") << "vt 1 1" << STATUS_OK;
    QTest::newRow("vertexTexture-many") << "vt 1 1 1 1 1 1 1" << STATUS_OK;
    QTest::newRow("normal-3") << "vn 1 1 1" << STATUS_OK;
    QTest::newRow("normal-many") << "vn 1 1 1 1 1 1 1" << STATUS_OK;
    QTest::newRow("face-3") << "v 1 1 1 \n f 1 1 1 " << STATUS_OK; // TODO add "end whitespace" check test
    QTest::newRow("face-many") << "v 1 1 1 \n f 1 1 1 1 1 1 1" << STATUS_OK;
}

void Parser::testNumArguments()
{
    QFETCH(QString, input);
    QFETCH(Newpson::Parsing::Obj::Status, status);

    Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
    QCOMPARE(parserResult.status, status);
}

void Parser::testEmptyFile_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<Newpson::Parsing::Obj::Status>("status");
    QTest::newRow("empty") << "" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("space") << "\u0020" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("spaces") << "\u0020\u0020\u0020\u0020\u0020\u0020\u0020" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("tab") << "\u000B" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("tabs") << "\u000B\u000B\u000B\u000B\u000B\u000B\u000B" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("newline") << "\n" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("newlines") << "\n\n\n\n\n\n\n" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("spaces+tabs+newlines") << "\u000B\u0020\u000B\u0020\u0020\n\u0020\u0020\n\n\u000B\u0020" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("trash-ascii") << "\n\n\000B\asdfasdf\0020dsfs\0020dfafn\nnsfn\n\n\u000B\n" << STATUS_ERROR_INPUT_EMPTY;
    QTest::newRow("trash-unicode") << "\u00e7\u008a\u00bd\u002d\u00dd\u00b7\u00ce\u0085\u007a\u00f2\u00b2\u00b0\u0097\u0057\u0072\u00f4\u0081\u0099\u00a2\u007d\u00cb" << STATUS_ERROR_INPUT_EMPTY;
}

void Parser::testEmptyFile()
{
    QFETCH(QString, input);
    QFETCH(Newpson::Parsing::Obj::Status, status);

    Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
    QCOMPARE(parserResult.status, status);
}

void Parser::testComponentsCoherence_data()
{
    QTest::addColumn<QString>("face");
    QTest::addColumn<Newpson::Parsing::Obj::Status>("status");
    QTest::newRow("111 111 111") << "f 1/1/1 1/1/1 1/1/1" << STATUS_OK;
    QTest::newRow("111 111 110") << "f 1/1/1 1/1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 111 101") << "f 1/1/1 1/1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 111 100") << "f 1/1/1 1/1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 110 111") << "f 1/1/1 1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 110 110") << "f 1/1/1 1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 110 101") << "f 1/1/1 1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 110 100") << "f 1/1/1 1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 101 111") << "f 1/1/1 1//1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 101 110") << "f 1/1/1 1//1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 101 101") << "f 1/1/1 1//1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 101 100") << "f 1/1/1 1//1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 100 111") << "f 1/1/1 1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 100 110") << "f 1/1/1 1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 100 101") << "f 1/1/1 1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("111 100 100") << "f 1/1/1 1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 111 111") << "f 1/1 1/1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 111 110") << "f 1/1 1/1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 111 101") << "f 1/1 1/1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 111 100") << "f 1/1 1/1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 110 111") << "f 1/1 1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 110 110") << "f 1/1 1/1 1/1" << STATUS_OK;
    QTest::newRow("110 110 101") << "f 1/1 1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 110 100") << "f 1/1 1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 101 111") << "f 1/1 1//1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 101 110") << "f 1/1 1//1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 101 101") << "f 1/1 1//1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 101 100") << "f 1/1 1//1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 100 111") << "f 1/1 1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 100 110") << "f 1/1 1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 100 101") << "f 1/1 1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("110 100 100") << "f 1/1 1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 111 111") << "f 1//1 1/1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 111 110") << "f 1//1 1/1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 111 101") << "f 1//1 1/1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 111 100") << "f 1//1 1/1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 110 111") << "f 1//1 1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 110 110") << "f 1//1 1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 110 101") << "f 1//1 1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 110 100") << "f 1//1 1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 101 111") << "f 1//1 1//1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 101 110") << "f 1//1 1//1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 101 101") << "f 1//1 1//1 1//1" << STATUS_OK;
    QTest::newRow("101 101 100") << "f 1//1 1//1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 100 111") << "f 1//1 1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 100 110") << "f 1//1 1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 100 101") << "f 1//1 1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("101 100 100") << "f 1//1 1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 111 111") << "f 1 1/1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 111 110") << "f 1 1/1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 111 101") << "f 1 1/1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 111 100") << "f 1 1/1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 110 111") << "f 1 1/1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 110 110") << "f 1 1/1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 110 101") << "f 1 1/1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 110 100") << "f 1 1/1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 101 111") << "f 1 1//1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 101 110") << "f 1 1//1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 101 101") << "f 1 1//1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 101 100") << "f 1 1//1 1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 100 111") << "f 1 1 1/1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 100 110") << "f 1 1 1/1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 100 101") << "f 1 1 1//1" << STATUS_ERROR_COMPONENTS_INCOHERENCE;
    QTest::newRow("100 100 100") << "f 1 1 1" << STATUS_OK;
}

void Parser::testComponentsCoherence()
{
    QFETCH(QString, face);
    QFETCH(Newpson::Parsing::Obj::Status, status);

    QString input = commonPart + face;

    Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
    QCOMPARE(parserResult.status, status);
}

void Parser::testUndefinedIndex_data()
{
    QTest::addColumn<QString>("face");
    QTest::addColumn<Newpson::Parsing::Obj::Status>("status");
    QTest::newRow("vertices-defined") << "f 1 2 3 4" << STATUS_OK;
    QTest::newRow("vertex-undefined") << "f 5" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("vertices-undefined") << "f 5 6 7 8" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("vertices-undefined+defined") << "f 5 1 2 3" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("vertices-defined+undefined") << "f 1 2 3 5" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("verticesTexture-defined") << "f 1/1 2/2 3/3 4/4" << STATUS_OK;
    QTest::newRow("vertexTexture-undefined") << "f 1/5" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("verticesTexture-undefined") << "f 1/5 2/6 3/7 4/8" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("verticesTexture-undefined+defined") << "f 1/5 2/2 3/3 4/4" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("verticesTexture-defined+undefined") << "f 1/1 2/2 3/3 4/5" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("normals-defined") << "f 1//1 2//1 3//1 4//1" << STATUS_OK;
    QTest::newRow("normal-undefined") << "f 1//7" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("normals-undefined") << "f 1//7 2//8 3//9 4//10" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("normals-undefined+defined") << "f 1//7 2//1 3//1 4//1" << STATUS_ERROR_UNDEFINED_INDEX;
    QTest::newRow("normals-defined+undefined") << "f 1//1 2//1 3//1 4//7" << STATUS_ERROR_UNDEFINED_INDEX;
}

void Parser::testUndefinedIndex()
{
    QFETCH(QString, face);
    QFETCH(Newpson::Parsing::Obj::Status, status);

    QString input = commonPart + face;

    Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
    QCOMPARE(parserResult.status, status);
}

void Parser::testParsedData()
{
    QString input(\
        "# Blender 4.3.1 \n" \
        "# www.blender.org \n" \
        "mtllib cube4.mtl \n" \
        "o Cube \n" \
        "v 1.000000 1.000000 -1.000000 \n" \
        "v 1.000000 -1.000000 -1.000000 \n" \
        "v 1.000000 1.000000 1.000000 \n" \
        "v 1.000000 -1.000000 1.000000 \n" \
        "v -1.000000 1.000000 -1.000000 \n" \
        "v -1.000000 -1.000000 -1.000000 \n" \
        "v -1.000000 1.000000 1.000000 \n" \
        "v -1.000000 -1.000000 1.000000 \n" \
        "vn -0.0000 1.0000 -0.0000 \n" \
        "vn -0.0000 -0.0000 1.0000 \n" \
        "vn -1.0000 -0.0000 -0.0000 \n" \
        "vn -0.0000 -1.0000 -0.0000 \n" \
        "vn 1.0000 -0.0000 -0.0000 \n" \
        "vn -0.0000 -0.0000 -1.0000 \n" \
        "vt 0.625000 0.500000 \n" \
        "vt 0.875000 0.500000 \n" \
        "vt 0.875000 0.750000 \n" \
        "vt 0.625000 0.750000 \n" \
        "vt 0.375000 0.750000 \n" \
        "vt 0.625000 1.000000 \n" \
        "vt 0.375000 1.000000 \n" \
        "vt 0.375000 0.000000 \n" \
        "vt 0.625000 0.000000 \n" \
        "vt 0.625000 0.250000 \n" \
        "vt 0.375000 0.250000 \n" \
        "vt 0.125000 0.500000 \n" \
        "vt 0.375000 0.500000 \n" \
        "vt 0.125000 0.750000 \n" \
        "s 0 \n" \
        "usemtl Material \n" \
        "f 1/1/1 5/2/1 7/3/1 3/4/1 \n" \
        "f 4/5/2 3/4/2 7/6/2 8/7/2 \n" \
        "f 8//3 7//3 5//3 6//3 \n" \
        "f 6/12/4 2/13/4 4/5/4 8/14/4 \n" \
        "f 2/13 1/1 3/4 4/5 \n" \
        "f 6 5 1 2 \n");

    const Newpson::Mesh mesh = Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
    QCOMPARE(parserResult.status, STATUS_OK);
    QCOMPARE(mesh.areLengthsOfFacesIndicesVecotorsEqualAndIndicesVerticesVectorIsValidAndIndicesVerticesTextureVectorIsValidAndIndicesNormalsVectorIsValidAndFaceVerticesIndicesValidAndFaceVerticesTextureInidicesValidAndEtc(), Mesh::VALIDATION_OK);

    const QVector<QVector3D> &vertices = mesh.vertices();
    QCOMPARE(vertices.length(), 8);
    QCOMPARE(vertices[0], QVector3D( 1.000000,  1.000000, -1.000000));
    QCOMPARE(vertices[1], QVector3D( 1.000000, -1.000000, -1.000000));
    QCOMPARE(vertices[2], QVector3D( 1.000000,  1.000000,  1.000000));
    QCOMPARE(vertices[3], QVector3D( 1.000000, -1.000000,  1.000000));
    QCOMPARE(vertices[4], QVector3D(-1.000000,  1.000000, -1.000000));
    QCOMPARE(vertices[5], QVector3D(-1.000000, -1.000000, -1.000000));
    QCOMPARE(vertices[6], QVector3D(-1.000000,  1.000000,  1.000000));
    QCOMPARE(vertices[7], QVector3D(-1.000000, -1.000000,  1.000000));

    const QVector<QVector2D> &verticesTexture = mesh.verticesTexture();
    QCOMPARE(verticesTexture.length(), 14);
    QCOMPARE(verticesTexture[ 0], QVector2D(0.625000, 0.500000));
    QCOMPARE(verticesTexture[ 1], QVector2D(0.875000, 0.500000));
    QCOMPARE(verticesTexture[ 2], QVector2D(0.875000, 0.750000));
    QCOMPARE(verticesTexture[ 3], QVector2D(0.625000, 0.750000));
    QCOMPARE(verticesTexture[ 4], QVector2D(0.375000, 0.750000));
    QCOMPARE(verticesTexture[ 5], QVector2D(0.625000, 1.000000));
    QCOMPARE(verticesTexture[ 6], QVector2D(0.375000, 1.000000));
    QCOMPARE(verticesTexture[ 7], QVector2D(0.375000, 0.000000));
    QCOMPARE(verticesTexture[ 8], QVector2D(0.625000, 0.000000));
    QCOMPARE(verticesTexture[ 9], QVector2D(0.625000, 0.250000));
    QCOMPARE(verticesTexture[10], QVector2D(0.375000, 0.250000));
    QCOMPARE(verticesTexture[11], QVector2D(0.125000, 0.500000));
    QCOMPARE(verticesTexture[12], QVector2D(0.375000, 0.500000));
    QCOMPARE(verticesTexture[13], QVector2D(0.125000, 0.750000));

    const QVector<QVector3D> &normals = mesh.normals();
    QCOMPARE(normals.length(), 6);
    QCOMPARE(normals[0], QVector3D(-0.0000,  1.0000, -0.0000));
    QCOMPARE(normals[1], QVector3D(-0.0000, -0.0000,  1.0000));
    QCOMPARE(normals[2], QVector3D(-1.0000, -0.0000, -0.0000));
    QCOMPARE(normals[3], QVector3D(-0.0000, -1.0000, -0.0000));
    QCOMPARE(normals[4], QVector3D( 1.0000, -0.0000, -0.0000));
    QCOMPARE(normals[5], QVector3D(-0.0000, -0.0000, -1.0000));

    const QVector<int> &indicesVertices = mesh.indicesVertices();
    const QVector<int> &indicesVerticesTexture = mesh.indicesVerticesTexture();
    const QVector<int> &indicesNormals = mesh.indicesNormals();
    QCOMPARE(indicesVertices, QVector<int>({0, 4, 6, 2, 3, 2, 6, 7, 7, 6, 4, 5, 5, 1, 3, 7, 1, 0, 2, 3, 5, 4, 0, 1}));
    QCOMPARE(indicesVerticesTexture, QVector<int>({0, 1, 2, 3, 4, 3, 5, 6, 11, 12, 4, 13, 12, 0, 3, 4}));
    QCOMPARE(indicesNormals, QVector<int>({0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3}));

    const QVector<int> &facesVertices = mesh.facesVertices();
    const QVector<int> &facesVerticesTexture = mesh.facesVerticesTexture();
    const QVector<int> &facesNormals = mesh.facesNormals();
    QCOMPARE(facesVertices, QVector<int>({4, 8, 12, 16, 20, 24}));
    QCOMPARE(facesVerticesTexture, QVector<int>({4, 8, 8, 12, 16, 16}));
    QCOMPARE(facesNormals, QVector<int>({4, 8, 12, 16, 16, 16}));
}

} // namespace Newpson::Parsing::Obj::Test
