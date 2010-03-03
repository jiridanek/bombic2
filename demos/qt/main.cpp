
#include <QApplication>
#include <QWidget>
#include <QGridLayout>

#include "mapview.h"
#include "mapobjectpalette.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QWidget * win = new QWidget;
	QGridLayout * layout = new QGridLayout(win);

	MapObjectPalette * palette = new MapObjectPalette(win);
	layout->addWidget(palette, 0, 1);

	MapView * mapView = new MapView(win, palette);
	layout->addWidget(mapView->getViewport(), 0, 0);

	win->show();

	return app.exec();
}