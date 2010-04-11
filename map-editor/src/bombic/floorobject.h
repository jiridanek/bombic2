/** @file floorobject.h
 * Objekt mapy typu floorobject je predstavovan tridou BombicFloorobject.
 */

#ifndef BOMBIC_FLOOROBJECT_H_GUARD_
#define BOMBIC_FLOOROBJECT_H_GUARD_

#include "map_object.h"

/** Objekt na zemi, tak jak ho zname ze hry.
 * Floorobject je objekt mapy, ktery muze mit vetsi rozmery,
 * nikdy vsak nepresahuje oblast, kterou zabira.
 * Floorobject muze byt na jednom policku s priserou ci hracem.
 * S objekty na zemi je mozne hybat i je mazat.
 */
class BombicFloorobject: public BombicMapObject {

	public:
		/// Konstrukce floorobjectu.
		BombicFloorobject(const QString & name, const QPixmap & pixmap,
				int width, int height);
		/// Kopie floorobjectu.
		BombicFloorobject(BombicFloorobject & srcFloorobject);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
		/// Jestli muze byt s jinym objektem na stejnem policku.
		virtual bool canBeWith(BombicMapObject * object);
		/// Jestli objekt blokuje generovani beden.
		virtual bool blocksBoxGenerating();
		/// Jestli objekt blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
};

#endif
