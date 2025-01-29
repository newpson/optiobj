#include <QTest>
#include <QString>
#include <QTextStream>
#include "Mesh.hpp"

namespace Newpson::Test {

class Mesh: public QObject
{
    Q_OBJECT
private slots:
    void parseGeometryVertex()
    {
        Newpson::Mesh mesh;
        auto status = Newpson::Mesh::read(mesh, PROJECT_ASSETS "/bad.obj");
        QVERIFY(status >= Newpson::Mesh::STATUS_ERROR);
    }
};

} // namespace Newpson::Test
