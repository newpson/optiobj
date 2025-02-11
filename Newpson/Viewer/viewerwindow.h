#ifndef NEWPSON_VIEWER_VIEWERWINDOW_H
#define NEWPSON_VIEWER_VIEWERWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "Newpson/Obj/Parser/parser.h"
#include "Newpson/Viewer/GLStorage/glstorage.h"
#include "Newpson/Viewer/GLWidget/glwidget.h"
#include "Newpson/Viewer/GLTreeWidget/gltreewidget.h"

namespace Newpson::Viewer {

class ViewerWindow: public QMainWindow
{
    Q_OBJECT

public:
    ViewerWindow(QWidget *parent = nullptr);

protected:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *verticalLayoutWidget;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonRemove;
    QPushButton *buttonLoad;
    QStatusBar *statusbar;

    GLWidget *glWidget;
    GLTreeWidget *glTreeWidget;
    GLStorage glStorage;

    void showParserErrorMessage(Newpson::ObjParser::ParserResult parserResult);

public slots:
    void onButtonLoadClicked();

private:
};

}

#endif
