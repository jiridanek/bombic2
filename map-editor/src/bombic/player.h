/** @file player.h
 * Objekt mapy typu hrac je predstavovan tridou BombicPlayer.
 */

#ifndef BOMBIC_PLAYER_H_GUARD_
#define BOMBIC_PLAYER_H_GUARD_

#include "map_object.h"

/** Hrac, tak jak ho zname ze hry.
 * Hrac je zvlastni objekt mapy, ktery zabira vzdy jedno policko.
 * Objekt v map-editoru reprezentuje startovni policko hrace,
 * nebo hracu kooperative modu.
 * V mape musi byt presne dany pocet hracu. To je zajisteno tim,
 * ze jsou do mapy jisti hraci pridani pri vygenerovani mapy a nemohou
 * z ni byt odstraneni. Mohou se premistovat.
 * Hrac muze byt na policku s dalsimi hraci nebo objektem na zemi.
 */
class BombicPlayer: public BombicMapObject {

	public:
		/// Konstrukce hrace.
		BombicPlayer(const QString & name, const QPixmap & pixmap);
		/// Kopie hrace.
		BombicPlayer(BombicPlayer & srcPlayer);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
		/// Jestli muze byt s jinym objektem na stejnem policku.
		virtual bool canBeWith(BombicMapObject * object);
		/// Jestli muze byt objekt odstranen z mapy.
		virtual bool canBeRemoved();
		/// Jestli ma byt objekt na policku razen.
		virtual bool sortOnField();

};

#endif
