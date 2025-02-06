#include <QTest>
#include "test/parser.h"

int main()
{
    Newpson::Parsing::Obj::Test::Parser test;
	QTest::qExec(&test);
}
