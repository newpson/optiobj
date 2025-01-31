#include "Mesh.hpp"
#include "obj/Parser.hpp"

#include <QVector>
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <QFile>

int main()
{
    Newpson::Mesh mesh;
    Newpson::Parsing::obj::Parser::ParserState parserStatus;

    parserStatus = Newpson::Parsing::obj::Parser::load(PROJECT_ASSETS "/ok/suzanne.obj", mesh);

    if (parserStatus.status != Newpson::Parsing::obj::Parser::STATUS_OK) {
        qDebug() << Newpson::Parsing::obj::Parser::statusToString(parserStatus.status) << "at";
        qDebug() << "line" << parserStatus.lineNumber;
        qDebug() << "column" << parserStatus.columnNumber;
    } else {
        qDebug() << "Everything is fine";
    }

    return 0;
}
