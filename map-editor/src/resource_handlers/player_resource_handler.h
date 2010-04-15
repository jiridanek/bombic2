/** @file player_resource_handler.h
 * Resource handler specializovany na hrace PlayerResourceHandler
 * umi zpracovat dokument a vytvorit objekt mapy typu hrac.
 */

#ifndef PLAYER_RESOURCE_HANDLER_H_GUARD_
#define PLAYER_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

/** Resource handler specializovany na objekty typu hrac.
 * Zpracovava zdroj reprezentujici hrace a vytvari
 * prototyp hrace.
 */
class PlayerResourceHandler: public MapObjectResourceHandler {

	public:
		/// Vytvorit prototyp hrace.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);
		/// Typ vytvareneho objektu.
		virtual BombicMapObject::Type type();
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);
};

#endif
