#include "QtVmbViewer.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QtVmbViewer w;
	w.setScaledContents( true );
	w.show();
	return a.exec();
}
