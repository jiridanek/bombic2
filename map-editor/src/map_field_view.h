/** @file map_field_view.h
 * Pohled na policko mapy @c MapFieldView je widget,
 * ktery zobrazi prehledne vsechny objekty na jednom policku mapy.
 *
 * Kazdy objekt na policku je potom reprezentovan instanci
 * @c MapFieldViewObject, ktery vizualizuje objekt a provadi s nim operace.
 */

#ifndef MAP_FIELD_VIEW_H_GUARD_
#define MAP_FIELD_VIEW_H_GUARD_

#include <QWidget>

#include "bombic/map.h"

class QScrollArea;
class QCheckBox;
class MapScene;
class MapObjectGenerator;
class MapFieldViewObject;

/** Pohled na policko mapy.
 * Widget, ktery zobrazuje, co je na jednom konkretnim policku.
 * Navic dava moznost s obsahem policka manipulovat (mazat, presouvat objekty,
 * nastatovat generovani objektu).
 */
class MapFieldView: public QWidget {

	Q_OBJECT

	public:
		/// Zkonstruuje pohled.
		explicit MapFieldView(QWidget * parent = 0);

		/// Obnoveni pohledu, pripadne zmena policka.
		void update(MapScene * scene);

	private:
		/// Odpojit checkboxy.
		void disconnectCheckboxes();
		/// Napojit checkboxy na policko mapy.
		void connectCheckboxes(BombicMap * map,
				const BombicMap::Field & field);
		/// Napojit checkbox na generovany objekt policka.
		void connectCheckbox(QCheckBox * checkbox,
				MapObjectGenerator * objGen);

		/// Posouvaci pole widgetu.
		QScrollArea * scrollArea_;
		/// Stary skrolovany kontejner, uchovavany kvuli widgetum
		/// v nem ulozenym. Pri tazeni objektu se totiz nejdrive
		/// obnovi pohled a pote se dokonci tazeni objektu.
		/// Pohled na tazeny objekt tedy musime zachovat dele,
		/// nez je ve skutecnosti zobrazovan.
		QWidget * oldScrollWidget_;

		/// Obecny widget, ve kterem jsou umisteny checkboxy pro generovani.
		QWidget * boxesWidget_;
		/// Tlacitko manipulujici s generovanou bednou policka.
		QCheckBox * generateBox_;
		/// Tlacitko manipulujici s generovanou priserou policka.
		QCheckBox * generateCreature_;
};



#endif
