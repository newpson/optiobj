!NEWPSON_VIEWER_GLWIDGET_PRI {
CONFIG += NEWPSON_VIEWER_GLWIDGET_PRI

INCLUDEPATH *= $$clean_path($$PWD/../../..)
include($$PWD/../Camera/Camera.pri)
include($$PWD/../GLStorage/GLStorage.pri)

HEADERS += \
    $$PWD/glwidget.h \

SOURCES += \
    $$PWD/glwidget.cpp \

}
