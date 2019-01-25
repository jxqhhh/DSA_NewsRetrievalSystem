#include "Widget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Widget w;
	w.setWindowTitle(QString::fromStdWString(L"мЬрвпбне"));
	w.show();
	return a.exec();
}
