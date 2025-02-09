#ifndef NEWPSON_VIEWER_VIEWERWINDOW_H
#define NEWPSON_VIEWER_VIEWERWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "Newpson/Viewer/ui_viewerwindow.h"

namespace Newpson::Ui {

class ViewerWindow: public QMainWindow, public Ui::ViewerWindow
{
    Q_OBJECT
public:
    ViewerWindow(QWidget *parent = nullptr);
};

}

#endif
