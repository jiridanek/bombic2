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
#include <QDockWidget>
#include <QObject>
#include <QDateTime>
#include <QWidget>
#include <QGridLayout>
#include <QSplitter>

#include "main_window.h"
#include "resource_handler.h"
#include "map_object_palette.h"
#include "map_view.h"

/**
 * Hlavni inicializator programu.
 * Vytvori Qt aplikaci, inicializuje okno, singletony.
 * @param argc
 * @param argv
 * @return Uspech programu.
 * @see QApplication, QApplication::exec()
 */
int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	// vytvorit nahodnou posloupnost
	qsrand( QDateTime::currentDateTime().toTime_t() );

	// konstrukce singletonu
	new MainWindow;
	new ResourceHandler(MAIN_WINDOW);
	new MapObjectPalette(MAIN_WINDOW);
	new MapView(MAIN_WINDOW);

	MAIN_WINDOW->show();
	return app.exec();
}
