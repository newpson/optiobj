!NEWPSON_VIEWER_PRI {
CONFIG += NEWPSON_VIEWER_PRI

INCLUDEPATH *= $$clean_path($$PWD/../..)
include($$PWD/../Mesh/Mesh.pri)
include($$PWD/../ObjParser/ObjParser.pri)

HEADERS += \
    $$PWD/newpsonviewerviewerwindow.h \
    $$PWD/newpsonviewerglwidget.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/glwidget.cpp \
    $$PWD/viewerwindow.cpp \

FORMS += \
    $$PWD/viewerwindow.ui \

}
