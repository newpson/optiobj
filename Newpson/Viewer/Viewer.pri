!NEWPSON_VIEWER_PRI {
CONFIG += NEWPSON_VIEWER_PRI

INCLUDEPATH *= $$clean_path($$PWD/../..)
include($$PWD/GLStorage/GLStorage.pri)
include($$PWD/GLWidget/GLWidget.pri)
include($$PWD/GLTreeWidget/GLTreeWidget.pri)

include($$PWD/../Mesh/Mesh.pri)
include($$PWD/../Obj/Parser/Parser.pri)

HEADERS += \
    $$PWD/viewerwindow.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/viewerwindow.cpp \

}
