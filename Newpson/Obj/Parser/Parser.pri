!NEWPSON_OBJ_PARSER_PRI {
CONFIG += NEWPSON_OBJ_PARSER_PRI

INCLUDEPATH *= $$clean_path($$PWD/../../..)
include($$PWD/../../Mesh/Mesh.pri)

SOURCES += \
    $$PWD/parser.cpp
HEADERS += \
    $$PWD/parser.h \
    $$PWD/parser-internal.h

}
