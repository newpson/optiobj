#include "Newpson/Viewer/viewerwindow.h"

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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "Newpson/Viewer/GLWidget/glwidget.h"
#include "Newpson/Obj/Parser/parser.h"
#include "Newpson/Mesh/mesh.h"

namespace Newpson::Viewer {

ViewerWindow::ViewerWindow(QWidget *parent):
    QMainWindow(parent)
{
    resize(800, 600);

    setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));

    centralwidget = new QWidget(this);
    horizontalLayout_2 = new QHBoxLayout(centralwidget);
    splitter = new QSplitter(centralwidget);
    splitter->setOrientation(Qt::Horizontal);
    glWidget = new GLWidget(splitter);
    splitter->addWidget(glWidget);
    verticalLayoutWidget = new QWidget(splitter);
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    glTreeWidget = new GLTreeWidget(verticalLayoutWidget);
    verticalLayout->addWidget(glTreeWidget);

    horizontalLayout = new QHBoxLayout();
    horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    buttonRemove = new QPushButton(verticalLayoutWidget);
    buttonRemove->setText(QCoreApplication::translate("MainWindow", "Remove", nullptr));

    horizontalLayout->addWidget(buttonRemove);

    buttonLoad = new QPushButton(verticalLayoutWidget);
    buttonLoad->setText(QCoreApplication::translate("MainWindow", "Load", nullptr));
    connect(buttonLoad, SIGNAL(clicked()), this, SLOT(onButtonLoadClicked()));

    horizontalLayout->addWidget(buttonLoad);

    verticalLayout->addLayout(horizontalLayout);

    splitter->addWidget(verticalLayoutWidget);

    horizontalLayout_2->addWidget(splitter);

    setCentralWidget(centralwidget);
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);
};

void ViewerWindow::showParserErrorMessage(Newpson::ObjParser::ParserResult parserResult)
{
    QMessageBox messageBox;
    QString statusString = QCoreApplication::translate("ViewerWindow", "Status: ", nullptr);
    statusString += Newpson::ObjParser::statusToString(parserResult.status);
    statusString += "\n\n";
    statusString += QCoreApplication::translate("ViewerWindow", "line ", nullptr);
    statusString += QString::number(parserResult.lineNumber);
    statusString += "\n";
    statusString += QCoreApplication::translate("ViewerWindow", "column ", nullptr);
    statusString += QString::number(parserResult.columnNumber);
    statusString += ".";
    messageBox.critical(0,
            QCoreApplication::translate("ViewerWindow", "Loading error", nullptr),
            statusString);
    messageBox.show();
}

void ViewerWindow::onButtonLoadClicked()
{
    QString fname = QFileDialog::getOpenFileName(this);
    Newpson::ObjParser::ParserResult parserResult;
    Mesh mesh = Newpson::ObjParser::load(fname, parserResult);
    if (parserResult.status != Newpson::ObjParser::STATUS_OK) {
        showParserErrorMessage(parserResult);
        return;
    }


    // glStorage.addMesh();
}

}
