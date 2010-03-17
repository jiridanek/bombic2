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
#include <QSplitter>

#include "resource_handler.h"
#include "map_object_palette.h"
#include "map_view.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	QWidget * win = new QWidget;
	QGridLayout * winLayout = new QGridLayout(win);

	// konstrukce singletonu
	new ResourceHandler(win);
	new MapObjectPalette(win);
	new MapView(win);

	// poskladat prvky gui do layoutu okna
	QSplitter * winSplitter = new QSplitter(win);
	winSplitter->addWidget(MAP_VIEW);
	winSplitter->addWidget(MAP_OBJECT_PALETTE);
	winLayout->addWidget(winSplitter, 0, 0);

	win->show();

	return app.exec();
}