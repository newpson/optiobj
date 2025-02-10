include(moc.prf)
include(uic.prf)

QT += core gui widgets testlib
equals(QT_MAJOR_VERSION, 6) {
    QT += openglwidgets
}
CONFIG += c++17 object_parallel_to_source
DEFINES += "PROJECT_ASSETS=\\\"$$PWD/assets\\\""
