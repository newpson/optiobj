#include <QTest>
#include <QString>
#include <QTextStream>
#include "obj/Parser.hpp"
#include "Mesh.hpp"

namespace Newpson::Parsing::obj::Test {

class Mesh: public QObject
{
    Q_OBJECT
private slots:
    void parseGeometryVertex()
    {
        Newpson::Mesh mesh;
        auto status = Newpson::Parsing::obj::Parser::load(PROJECT_ASSETS "/bad.obj", mesh);

        QVERIFY(status >= Newpson::Parsing::obj::Parser::STATUS_OK);
    }
};

} // namespace Newpson::Test
