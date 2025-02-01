#include <QString>
#include <QTest>
#include <QTextStream>
#include "mesh.hpp"
#include "obj/parser.hpp"

#define COMMON_PART "v 0 0 0 \n v 1 1 1 \n v 2 2 2 \n vt 1 0.9 \n vt 0.8 0.7 \n vt 0.6 0.5 \n vn 1 2 3 \n vn 2 1 3 \n vn 2 3 1 \n"

namespace Newpson::Parsing::OBJ::Test {

class Parser: public QObject
{
    Q_OBJECT
private slots:
    void testNumArguments()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_EXPECTED_FLOAT;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vt 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vt 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vn 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vn 1 1"), mesh).status == errorCode);
        errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_EXPECTED_INTEGER;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n v 1 1 1 \n v 1 1 1 \n f 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n v 1 1 1 \n v 1 1 1 \n f 1 2"), mesh).status == errorCode);
        errorCode = Newpson::Parsing::OBJ::Parser::STATUS_OK;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 1 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vt 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vt 1 1 1 1 1 1"), mesh).status == errorCode);
        // normals storage is not implemeneted
        // QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vn 1 1 1"), mesh).status == errorCode);
        // QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("vn 1 1 1 1 1 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n v 1 1 1 \n v 1 1 1 \n f 1 2 3"), mesh).status == errorCode);
    }

//    void testLineType()
//    {
//        // friend class requirement ?
//    }

    void testEmptyFile()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_INPUT_EMPTY;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(""), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("\u0020"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("\u0020\u0020\u0020\u0020\u0020\n\u0020\u0020"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("\u000B\u0020\u000B\u0020\u0020\n\u0020\u0020\n\n"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("\\n\asdfasdfdsfsdfafn\nnsfn\n\n"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("\u00e7\u008a\u00bd\u002d\u00dd\u00b7\u00ce\u0085\u007a\u00f2\u00b2\u00b0\u0097\u0057\u0072\u00f4\u0081\u0099\u00a2\u007d\u00cb"), mesh).status == errorCode);
    }

    void testComponentsAssymetry()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_COMPONENTS_ASSYMETRY;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1 2/2 3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/ 2/2 3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2 3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2/ 3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2/2 3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2/2 3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2 3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1//1 2/2/2 3/3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2//2 3/3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2/2/2 3//3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1//1 2//2 3/3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2//2 3//3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2// 3/3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2// 3/3/3"), mesh).status == errorCode);

        errorCode = Newpson::Parsing::OBJ::Parser::STATUS_OK;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/1 2/2/2 3/3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1//1 2//2 3//3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1// 2// 3//"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/ 2/2/ 3/3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1 2/2 3/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/ 2/ 3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1 2 3"), mesh).status == errorCode);
    }

    void testUndefinedIndex()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_UNDEFINED_INDEX;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("f 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("f -42"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n f 0 1 1"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n f 1/1/ 1// 1//"), mesh).status == errorCode);
        // normals storage is not implemeneted
        // QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream("v 1 1 1 \n f 1//1 1// 1//"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/1/ 1/1/ 1/4/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/4/ 1/1/ 1//"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/4"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/3/ 1/3/ 4/3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/3/ 4/3/ 1/3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 4/3/ 1/3/ 1/3/"), mesh).status == errorCode);
    }

    void testExpectedInteger()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_ERROR_EXPECTED_INTEGER;
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/3/ 1/3/ /3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f /3/ /3/ 7000/3/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f // 1/2/3 1/2/3"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f /2/ /2/ /2/"), mesh).status == errorCode);
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(QTextStream(COMMON_PART "f 1/2/2 1/2/2 /2/"), mesh).status == errorCode);
    }

    void testData()
    {
        Newpson::Mesh mesh;
        Newpson::Parsing::OBJ::Parser::Status errorCode = Newpson::Parsing::OBJ::Parser::STATUS_OK;
        // Values were verified in blender, link to model https://free3d.com/3d-model/bugatti-chiron-2017-model-31847.html
        QVERIFY(Newpson::Parsing::OBJ::Parser::load(PROJECT_ASSETS "/big/bugatti.obj", mesh).status == errorCode);
        qDebug() << mesh.geometryVertices.length(); // 744213
        QVERIFY(mesh.geometryVertices.length() == 744211); // ? does not pass; may be blender truncates some of the vertices? TODO
        QVERIFY(mesh.geometryFaces.length() == 759373); // OK
    }
};

} // namespace Newpson::Test
