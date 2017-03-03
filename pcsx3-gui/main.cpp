#include "pcsx3gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	pcsx3gui w;
	w.show();
	return a.exec();
}
