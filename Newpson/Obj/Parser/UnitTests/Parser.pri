!NEWPSON_OBJ_PARSER_UNITTESTS_PRI {
CONFIG += NEWPSON_OBJ_PARSER_UNITTESTS_PRI

INCLUDEPATH *= $$clean_path($$PWD/../../../..)
include($$PWD/../Parser.pri)
include($$PWD/../../../Mesh/Mesh.pri)

HEADERS += \
    $$PWD/parser.h

SOURCES += \
    $$PWD/parser.cpp

}
