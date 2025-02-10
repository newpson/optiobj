!NEWPSON_VIEWER_PRI {
CONFIG += NEWPSON_VIEWER_PRI

INCLUDEPATH *= $$clean_path($$PWD/../..)
include($$PWD/../Mesh/Mesh.pri)
include($$PWD/../Obj/Parser/Parser.pri)

HEADERS += \
    $$PWD/camera.h \
    $$PWD/viewerwindow.h \
    $$PWD/glwidget.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/camera.cpp \
    $$PWD/glwidget.cpp \
    $$PWD/viewerwindow.cpp \

FORMS += \
    $$PWD/viewerwindow.ui \

}
