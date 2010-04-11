/** @file creature_resource_handler.h
 * Resource handler specializovany na prisery CreatureResourceHandler
 * umi zpracovat dokument a vytvorit objekt mapy typu prisera.
 */

#ifndef CREATURE_RESOURCE_HANDLER_H_GUARD_
#define CREATURE_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicCreature;

/** Resource handler specializovany na objekty typu prisera.
 * Zpracovava zdroj reprezentujici priseru a vytvari
 * prototyp teto prisery.
 */
class CreatureResourceHandler: public MapObjectResourceHandler {

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
