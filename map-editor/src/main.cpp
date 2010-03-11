/**
 * Vitejte do editoru map hry Bombic2.
 * Pomoci editoru map muzete modifikovat starsi a dokonce i tvorit vlastni
 * mapy do hry Bombic2. Mapa se vzdy sklada z pozadi (pozadi vsech policek
 * a okrajove zdi) a objektu mapy (zdi, bedny, prisery, hraci, bonusy).
 * Tyto prvky jsou pripravene jako XML soubory, nazev souboru vzdy koresponduje
 * s nazvem prvku. V editoru muzete tyto prvky z XML souboru nahravat
 * a vkladat do mapy. Starsi mapy muzete otevirat (taktez z XML souboru)
 * a modifikovat. Pote ukladat zpet, nebo jako novou mapu do noveho souboru.
 * Pokud nevite co znamena zkratka XML, budte bez starosti,
 * editor udela vse za vas, Vy budete pouze klikat. Je velice intuitivni
 * a prehledny.
 */

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QGraphicsView>

#include "resource_handler.h"
#include "map_object_palette.h"
#include "map_view.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	QWidget * win = new QWidget;
	QGridLayout * layout = new QGridLayout(win);

	// konstrukce singletonu
	new ResourceHandler(win);
	MapObjectPalette * palette = new MapObjectPalette(win);
	MapView * mapView = new MapView(win);

	// poskladat prvky gui do layoutu okna
	layout->addWidget(mapView->getViewport(), 1, 0);
	layout->addWidget(palette, 1, 1);

	win->show();

	return app.exec();
}