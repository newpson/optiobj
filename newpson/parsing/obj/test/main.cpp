#include "Mesh.h"

#include <QTest>

int main()
{
    Newpson::Parsing::obj::Test::Mesh test;
	QTest::qExec(&test);
}
