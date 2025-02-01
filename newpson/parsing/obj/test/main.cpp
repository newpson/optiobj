#include "parser.h"

#include <QTest>

int main()
{
    Newpson::Parsing::OBJ::Test::Parser test;
	QTest::qExec(&test);
}
