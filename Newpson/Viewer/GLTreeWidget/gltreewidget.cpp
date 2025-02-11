#include "Newpson/Viewer/GLTreeWidget/gltreewidget.h"

#include <QWidget>
#include <QSurfaceFormat>
#include <QVector>
#include <QVector3D>
#include <QMouseEvent>
#include <QGuiApplication>

#include "Newpson/Obj/Parser/parser.h"
#include "Newpson/Mesh/mesh.h"

namespace Newpson::Viewer {

GLTreeWidget::GLTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{}

}
