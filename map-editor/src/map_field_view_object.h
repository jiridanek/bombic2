/** @file map_field_view_object.h
 * Kazdy objekt v @c MapFieldView je reprezentovan instanci
 * @c MapFieldViewObject, ktery vizualizuje objekt a provadi s nim operace.
 */

#ifndef MAP_FIELD_VIEW_OBJECT_H_GUARD_
#define MAP_FIELD_VIEW_OBJECT_H_GUARD_

#include <QLabel>

#include <bombic/map.h>

class QEvent;

class MapFieldViewObject: public QLabel {

	Q_OBJECT

	public:
		/// Zkonstruuje pohled.
		explicit MapFieldViewObject(BombicMapObject * mapObject, QWidget * parent = 0);

	private:
		BombicMapObject * object_;

};


#endif
