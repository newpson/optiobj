#include <QApplication>
#include "newpsonviewerviewerwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

    Newpson::Ui::ViewerWindow viewerWindow;
	viewerWindow.show();

	return app.exec();
}

