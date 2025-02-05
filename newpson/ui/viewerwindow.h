#ifndef NEWPSON_UI_VIEWERWINDOW_H
#define NEWPSON_UI_VIEWERWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "ui_viewerwindow.h"

class ViewerWindow: public QMainWindow, Ui::ViewerWindow
{
    Q_OBJECT
public:
    ViewerWindow(QWidget *parent = nullptr);
};

#endif
