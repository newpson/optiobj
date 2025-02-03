#include "parser.h"

#include <QTest>

int main()
{
    Newpson::Parsing::Obj::Test::Parser test;
	QTest::qExec(&test);
}
