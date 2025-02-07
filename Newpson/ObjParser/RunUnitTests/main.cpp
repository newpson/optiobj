#include <QTest>
#include "UnitTests/newpsonobjparser.h"

int main()
{
    Newpson::Parsing::Obj::Test::Parser test;
	QTest::qExec(&test);
}
