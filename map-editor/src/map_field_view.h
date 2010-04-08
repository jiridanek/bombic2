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
class BombicGeneratedObject;

/** Pohled na policko mapy.
 */
class MapFieldView: public QWidget {

	Q_OBJECT

	public:
		/// Zkonstruuje pohled.
		explicit MapFieldView(QWidget * parent = 0);

		void update(MapScene * scene);
	private:
		void disconnectCheckboxes();
		void connectCheckboxes(BombicMap * map,
				const BombicMap::Field & field);
		void connectCheckbox(QCheckBox * checkbox,
				BombicGeneratedObject * genObj);

		QScrollArea * scrollArea_;
		QCheckBox * generateBox_;
		QCheckBox * generateCreature_;
};



#endif
