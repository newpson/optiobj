#include <QString>
#include <QTest>
#include <QTextStream>
#include "mesh.hpp"
#include "obj/parser.hpp"

Q_DECLARE_METATYPE(Newpson::Parsing::Obj::Status)

namespace Newpson::Parsing::Obj::Test {

class Parser: public QObject
{
    Newpson::Parsing::Obj::ParserResult parserResult;

    Q_OBJECT
private slots:
    void testNumArguments_data()
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
        QTest::newRow("face-3") << "v 1 1 1 \n f 1 1 1 " << STATUS_OK;
        QTest::newRow("face-many") << "v 1 1 1 \n f 1 1 1 1 1 1 1" << STATUS_OK;
    }

    void testNumArguments()
    {
        QFETCH(QString, input);
        QFETCH(Newpson::Parsing::Obj::Status, status);

        Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
        QCOMPARE(parserResult.status, status);
    }

    void testEmptyFile_data()
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

    void testEmptyFile()
    {
        QFETCH(QString, input);
        QFETCH(Newpson::Parsing::Obj::Status, status);

        Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
        QCOMPARE(parserResult.status, status);
    }

    void testComponentsCoherence_data()
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

    void testComponentsCoherence()
    {
        QFETCH(QString, face);
        QFETCH(Newpson::Parsing::Obj::Status, status);

        QString input(
            "v  0 0 0 \n" \
            "v  0 0 1 \n" \
            "v  0 1 0 \n" \
            "v  0 1 1 \n" \
            "vt 0 0   \n" \
            "vt 0 1   \n" \
            "vt 1 0   \n" \
            "vt 1 1   \n" \
            "vn 0 0 1 \n" \
            "vn 0 1 0 \n" \
            "vn 1 0 0 \n"
        );
        input.append(face);

        Newpson::Parsing::Obj::load(QTextStream(&input), parserResult);
        QCOMPARE(parserResult.status, status);
    }

//    void testUndefinedIndex()
//    {
//        Newpson::Mesh mesh;
//        Newpson::Parsing::Obj::Parser::Status errorCode = Newpson::Parsing::Obj::Parser::STATUS_ERROR_UNDEFINED_INDEX;
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream("f 1"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream("f -42"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream("v 1 1 1 \n f 0 1 1"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream("v 1 1 1 \n f 1/1/ 1// 1//"), mesh).status == errorCode);
//        // normals storage is not implemeneted
//        // QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n f 1//1 1// 1//"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/1/ 1/1/ 1/4/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/4/ 1/1/ 1//"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/4"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/3/ 1/3/ 4/3/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/3/ 4/3/ 1/3/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 4/3/ 1/3/ 1/3/"), mesh).status == errorCode);
//    }

//    void testExpectedInteger()
//    {
//        Newpson::Mesh mesh;
//        Newpson::Parsing::Obj::Parser::Status errorCode = Newpson::Parsing::Obj::Parser::STATUS_ERROR_EXPECTED_INTEGER;
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/3/ 1/3/ /3/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f /3/ /3/ 7000/3/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f // 1/2/3 1/2/3"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f /2/ /2/ /2/"), mesh).status == errorCode);
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(QTextStream(COMMON_PART "f 1/2/2 1/2/2 /2/"), mesh).status == errorCode);
//    }

//    void testData()
//    {
//        Newpson::Mesh mesh;
//        Newpson::Parsing::Obj::Parser::Status errorCode = Newpson::Parsing::Obj::Parser::STATUS_OK;
//        // Values were verified in blender, link to model https://free3d.com/3d-model/bugatti-chiron-2017-model-31847.html
//        QVERIFY(Newpson::Parsing::Obj::Parser::load(PROJECT_ASSETS "/big/bugatti.obj", mesh).status == errorCode);
//        qDebug() << mesh.numGeometry(); // 744213
//        QVERIFY(mesh.numTextures() == 744211); // ? does not pass; may be blender truncates some of the vertices? TODO
//        QVERIFY(mesh.numFaces() == 759373); // OK
//    }
};

} // namespace Newpson::Test
