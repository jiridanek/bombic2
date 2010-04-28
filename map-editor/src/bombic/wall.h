/** @file wall.h
 * Objekt mapy typu zed je predstavovan tridou @c BombicWall.
 */

#ifndef BOMBIC_WALL_H_GUARD_
#define BOMBIC_WALL_H_GUARD_

#include "map_object.h"

/** Zed, tak jak ji zname ze hry.
 * Zed je objekt mapy, ktery muze mit vetsi rozmery,
 * definovany presah (toplapping).
 * Zdi se deli do zdi pozadi (obvodove) a obycejnych zdi.
 * Aby bylo mozne udelat obvodove zdi ruznorodejsi,
 * je povoleno davat obycejne zdi pres obvodove.
 * S obvodovymi zdmi neni mozne hybat ani je odstranovat z mapy.
 */
class BombicWall: public BombicMapObject {

	public:
		/// Konstrukce zdi.
		BombicWall(const QString & name, const QPixmap & pixmap,
				int width, int height, int toplapping);
		/// Kopie zdi.
		BombicWall(BombicWall & srcWall);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
		/// Jestli muze byt s jinym objektem na stejnem policku.
		virtual bool canBeWith(BombicMapObject * object);
		/// Jestli muze byt premistovana.
		virtual bool canBeDragged();
		/// Jestli muze byt odstranena z mapy.
		virtual bool canBeRemoved();
		/// Jestli se jedna o obvodovou zed (zed pozadi).
		bool isBackground();

};

#endif
