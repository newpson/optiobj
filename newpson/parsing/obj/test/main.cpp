#include "Parser.h"

#include <QTest>

int main()
{
    Newpson::Parsing::obj::Test::Parser test;
	QTest::qExec(&test);
}
