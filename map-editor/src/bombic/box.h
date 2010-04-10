/** @file box.h
 * Objekt mapy typu box je predstavovan tridou BombicBox.
 */

#ifndef BOMBIC_BOX_H_GUARD_
#define BOMBIC_BOX_H_GUARD_

#include "map_object.h"

/** Box, tak jak ji zname ze hry.
 * Box je objekt mapy, ktery muze mit vetsi rozmery,
 * definovany presah (toplapping).
 * Box muze byt na jednom policku pouze sam.
 * S boxy je mozne hybat i je mazat.
 */
class BombicBox: public BombicMapObject {

	public:
		/// Konstrukce boxu.
		BombicBox(const QString & name, const QPixmap & pixmap,
				int width, int height, int toplapping);
		/// Kopie boxu.
		BombicBox(BombicBox & srcBox);
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
};

#endif
