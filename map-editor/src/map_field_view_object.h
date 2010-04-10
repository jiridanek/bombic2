/** @file map_field_view_object.h
 * Kazdy objekt v @c MapFieldView je reprezentovan instanci
 * @c MapFieldViewObject, ktery vizualizuje objekt a provadi s nim operace.
 */

#ifndef MAP_FIELD_VIEW_OBJECT_H_GUARD_
#define MAP_FIELD_VIEW_OBJECT_H_GUARD_

#include <QLabel>

class BombicMapObject;
class QMouseEvent;

/** Vizualizace pevne umisteneho objektu mapy v detailu.
 * Detailni zobrazeni policka mapy @p MapFieldView zobrazuje
 * objekty mapy na zvolenem policku. K zobrazeni a praci s pevne
 * umistenymi objekty slouzi prave tato trida reprezentujici jeden
 * takovy objekt.
 */
class MapFieldViewObject: public QLabel {

	Q_OBJECT

	public:
		/// Zkonstruuje pohled.
		explicit MapFieldViewObject(BombicMapObject * mapObject,
				QWidget * parent = 0);

	protected:
		/// Handler pohybu mysi.
		virtual void mouseMoveEvent(QMouseEvent * event);
		/// Handler dvojkliku.
		virtual void mouseDoubleClickEvent(QMouseEvent * event);

	private:
		/// Reprezentovany objekt umisteny v mape.
		BombicMapObject * object_;

};


#endif
