include(moc.prf)
include(uic.prf)

QT += core gui widgets openglwidgets testlib
CONFIG += c++17 object_parallel_to_source
DEFINES += "PROJECT_ASSETS=\\\"$$PWD/assets\\\""
