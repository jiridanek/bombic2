/** @file creature.h
 * Objekt mapy typu prisera je predstavovan tridou BombicCreature.
 */

#ifndef BOMBIC_CREATURE_H_GUARD_
#define BOMBIC_CREATURE_H_GUARD_

#include "map_object.h"

/** Prisera, tak jak ji zname ze hry.
 * Prisera je objekt mapy, ktery zabira vzdy jedno policko.
 * Prisera muze byt na policku s dalsimi priserami nebo objektem na zemi.
 * S priserami je mozne hybat i je mazat.
 */
class BombicCreature: public BombicMapObject {

	public:
		/// Konstrukce prisery.
		BombicCreature(const QString & name, const QPixmap & pixmap);
		/// Kopie prisery.
		BombicCreature(BombicCreature & srcCreature);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
		/// Jestli muze byt s jinym objektem na stejnem policku.
		virtual bool canBeWith(BombicMapObject * object);
		/// Jestli objekt blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
		/// Jestli ma byt objekt na policku razen.
		virtual bool sortOnField();

};

#endif
