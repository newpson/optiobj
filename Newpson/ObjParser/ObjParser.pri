!NEWPSON_OBJPARSER_PRI {
CONFIG += NEWPSON_OBJPARSER_PRI

INCLUDEPATH *= $$clean_path($$PWD/..)
include($$PWD/../Mesh/Mesh.pri)

SOURCES += \
    $$PWD/objparser.cpp
HEADERS += \
    $$PWD/objparser.h \
    $$PWD/objparserinternal.h

}
