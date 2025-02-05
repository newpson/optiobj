#ifndef NEWPSON_UI_VIEWERWINDOW_H
#define NEWPSON_UI_VIEWERWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "ui_viewerwindow.h"

namespace Newpson::Ui {

class ViewerWindow: public QMainWindow, public Ui::ViewerWindow
{
    Q_OBJECT
public:
    ViewerWindow(QWidget *parent = nullptr);
};

} // namespace Newpson::Ui

#endif
