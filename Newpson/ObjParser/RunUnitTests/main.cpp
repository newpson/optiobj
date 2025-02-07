#include <QTest>
#include "UnitTests/newpsonobjparser.h"

int main()
{
    Newpson::ObjParser::Test::NewpsonObjParserTest test;
	QTest::qExec(&test);
}
