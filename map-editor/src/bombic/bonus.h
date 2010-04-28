/** @file bonus.h
 * Objekt mapy typu bonus je predstavovan tridou @c BombicBonus.
 */

#ifndef BOMBIC_BONUS_H_GUARD_
#define BOMBIC_BONUS_H_GUARD_

#include "map_object.h"

/** Bonus (umisteny v mape), tak jak ho zname ze hry.
 * Bonus je objekt mapy, ktery zabira vzdy jedno policko a nema zadny presah.
 * Bonus z hlediska editoru map muze byt na jednom policku pouze s bednou,
 * a to napevno umistenou nebo generovanou. Uzivatel si nemuze vybrat,
 * kde budou bonusy umisteny, proto nejsou znazorneny v palete objektu a nedaji
 * se napevno umistit do mapy. Bonusy jsou tedy pouze generovane.
 */
class BombicBonus: public BombicMapObject {

	public:
		/// Konstrukce bedny.
		BombicBonus(const QString & name, const QPixmap & pixmap);
		/// Kopie bonusu.
		BombicBonus(BombicBonus & srcBonus);
		/// Vytvorit kopii objektu.
		virtual BombicMapObject * createCopy();
		/// Typ objektu.
		virtual BombicMapObject::Type type();
		/// Umistit graficky prvek.
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position);
};

#endif
