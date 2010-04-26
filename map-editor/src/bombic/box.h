/** @file box.h
 * Objekt mapy typu bedna je predstavovan tridou BombicBox.
 */

#ifndef BOMBIC_BOX_H_GUARD_
#define BOMBIC_BOX_H_GUARD_

#include "map_object.h"

/** Bedna, tak jak ji zname ze hry.
 * Bedna je objekt mapy, ktery muze mit vetsi rozmery,
 * definovany presah (toplapping).
 * Bedna muze byt na jednom policku pouze sama.
 * S bednami je mozne hybat i je mazat.
 */
class BombicBox: public BombicMapObject {

	public:
		/// Konstrukce bedny.
		BombicBox(const QString & name, const QPixmap & pixmap,
				int width, int height, int toplapping);
		/// Kopie bedny.
		BombicBox(BombicBox & srcBox);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
		/// Jestli objekt odblokovava generovani bonusu.
		virtual bool unblocksBonusGenerating();
};

#endif
