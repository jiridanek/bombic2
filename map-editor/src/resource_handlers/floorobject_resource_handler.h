/** @file floorobject_resource_handler.h
 * Resource handler specializovany na objekty na zemi
 * @c FloorobjectResourceHandler umi zpracovat dokument
 * a vytvorit objekt mapy typu objekt na zemi.
 */

#ifndef FLOOROBJECT_RESOURCE_HANDLER_H_GUARD_
#define FLOOROBJECT_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicFloorobject;

/** Resource handler specializovany na objekty typu objekt na zemi.
 * Zpracovava zdroj reprezentujici objekt na zemi a vytvari
 * prototyp tohoto objektu.
 */
class FloorobjectResourceHandler: public MapObjectResourceHandler {

	public:
		/// Vytvorit prototyp objektu na zemi.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);
		/// Typ vytvareneho objektu.
		virtual BombicMapObject::Type type();
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);
};

#endif
