!NEWPSON_PARSING_OBJ_PARSER_UNITTESTS_PRI {
CONFIG += NEWPSON_PARSING_OBJ_PARSER_UNITTESTS_PRI

INCLUDEPATH *= $$clean_path($$PWD/..)
include($$PWD/../Parser.pri)
include($$PWD/../../../Mesh.pri)

SOURCES += \
    $$PWD/parser.cpp
HEADERS += \
    $$PWD/parser.h

}
