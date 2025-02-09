#include <QTest>
#include "Newpson/Obj/Parser/UnitTests/parser.h"

int main()
{
    Newpson::ObjParser::Test::Parser test;
	QTest::qExec(&test);
}
