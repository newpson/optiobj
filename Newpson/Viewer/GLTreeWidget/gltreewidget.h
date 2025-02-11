#ifndef NEWPSON_VIEWER_GLTREEWIDGET_H
#define NEWPSON_VIEWER_GLTREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include "Newpson/Mesh/mesh.h"
#include "Newpson/Viewer/GLStorage/glstorage.h"

namespace Newpson::Viewer {

class GLTreeWidget: public QTreeWidget
{
    Q_OBJECT
public:
    GLTreeWidget(QWidget *parent = nullptr);

};

}

#endif
