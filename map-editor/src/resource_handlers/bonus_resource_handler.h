/** @file bonus_resource_handler.h
 * Resource handler specializovany na bonus @c BonusResourceHandler
 * umi zpracovat dokument a vytvorit objekt mapy typu bonus.
 */

#ifndef BONUS_RESOURCE_HANDLER_H_GUARD_
#define BONUS_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicBonus;

/** Resource handler specializovany na objekty typu bonus.
 * Zpracovava zdroj reprezentujici bonus a vytvari
 * prototyp tohoto bonusu.
 */
class BonusResourceHandler: public MapObjectResourceHandler {

	public:
		/// Vytvorit prototyp prisery.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);
		/// Typ vytvareneho objektu.
		virtual BombicMapObject::Type type();
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);
};

#endif
