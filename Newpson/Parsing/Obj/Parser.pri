!NEWPSON_PARSER_OBJ_PRI {
CONFIG += NEWPSON_PARSER_OBJ_PRI

INCLUDEPATH *= $$clean_path($$PWD/..)
include($$MT_NEWPSON_MESH)

SOURCES += \
    $$PWD/parser.cpp
HEADERS += \
    $$PWD/parser.h \
    $$PWD/parser-internal.h

}
