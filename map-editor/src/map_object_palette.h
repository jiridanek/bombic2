/** @file map_object_palette.h
 * Definice singletonu MapObjectPalette,
 * ktery shromazduje nactene objekty mapy a nabizi je uzivateli
 * k interakci.
 */

#ifndef MAP_OBJECT_PALETTE_H_GUARD_
#define MAP_OBJECT_PALETTE_H_GUARD_

#include <QWidget>
#include <QString>
#include <QHash>
#include <QPoint>
#include <singleton.h>

#include "bombic/map_object.h"

/// Zkratka pro ziskani instance singletonu MapObjectPalette
#define MAP_OBJECT_PALETTE SINGLETON_SHORTCUT(MapObjectPalette)

class QTabWidget;
class QLayout;
class QSignalMapper;
class QPushButton;

/** Paleta objektù mapy.
 * Widget, ktery nabizi uzivateli k interakci
 * vsechny objekty mapy, ktere jiz byli nacteny.
 * Poskytuje take vstupni bod uzivatelskeho rozhrani
 * k nacitaci dodatecnych objektu mapy.
 * Widget se sklada z nekolika tabu, ve kterych jsou
 * roztrizeny objekty podle typu a tlacitko
 * pro nacteni novych objektu.
 * Objekty jsou v uzivatelskem rozhrani jako tlacitka,
 * ktere kdyz uzivatel zmackne (vybere) muze jednoduse
 * prislusny objekt vkladat.
 */
class MapObjectPalette: public QWidget {

	Q_OBJECT

	SINGLETON_DEFINITION(MapObjectPalette)

	public:
		/// Konstruuje paletu.
		MapObjectPalette(QWidget * parent = 0);
		/// Destruuje paletu.
		~MapObjectPalette();

		/// Objekt podle jmena.
		BombicMapObject * getObject(const QString & objectName);

		/// Pridat objekt.
		void addObject(BombicMapObject * object);

	signals:
		/// Zrusen vyber objektu.
		void objectUnselected();
		/// Objekt vybran.
		void objectSelected(BombicMapObject *);

	public slots:
		/// Vybrat objekt podle jmena.
		void selectObject(const QString & objectName);
		/** Vybrat objekt podle objektu.
		 * Objekt je vybran podle jmena objektu @p object.
		 * @param object pointer na platny objekt,
		 * jehoz jmeno bude pouzito
		 */
		inline void selectObject(BombicMapObject * object) {
			selectObject(object->name());
		}
		/// Zrusit vyber objektu.
		void unselectObject();

	private slots:
		/// Tlacitko objektu stisknuto (reakce na stisknuti).
		void objectButtonClicked(int objectPaletteIndex);

	private:
		/// Stranka QTabWidgetu, odpovida typu objektu.
		enum Page {
			wallPage, boxPage, creaturePage, floorobjectPage
		};

		/// Konstrukce tabu.
		void tabsConstruct();
		/// Pridat stranku (tab).
		void tabsAddPage(MapObjectPalette::Page pageIndex,
			const QString & tabLabel);

		/// Vybrat objekt.
		void selectObject(int objectPaletteIndex);
		/// Tlacitko objektu.
		QPushButton * getObjectButton(int objectPaletteIndex);

		/// Typ tabu.
		typedef struct {
			/// Pointer na widget s taby.
			QTabWidget * widget;
			/// Stranky (jejich layouty) tabu.
			QHash<Page, QLayout *> layouts;
		} Tabs;
		/// Taby s objekty.
		Tabs tabs_;
		/// Mapovani signalu tlacitek objektu.
		QSignalMapper * signalMapper_;
		/// Seznam objektu palety.
		QList<BombicMapObject *> objectPalette_;
		/// Indexy  objektu podle jmena objektu.
		QHash<QString, int> objectIndexesByName_;

		/// Index vybraneho objektu.
		int selectedObjectIndex_;

};

#endif
