!NEWPSON_OBJPARSER_UNITTESTS_PRI {
CONFIG += NEWPSON_OBJPARSER_UNITTESTS_PRI

INCLUDEPATH *= $$clean_path($$PWD/..)
include($$PWD/../ObjParser.pri)
include($$PWD/../../Mesh/Mesh.pri)

HEADERS += \
    $$PWD/newpsonobjparser.h

SOURCES += \
    $$PWD/tests.cpp

}
