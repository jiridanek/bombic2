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

#include <bombic/map.h>

class QEvent;

/** Pohled na policko mapy.
 */
class MapFieldView: public QWidget {

	Q_OBJECT

	public:
		/// Zkonstruuje pohled.
		explicit MapFieldView(QWidget * parent = 0);

	private:

};



#endif
