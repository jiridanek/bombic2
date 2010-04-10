/** @file box.h
 * Resource handler specializovany na bedny BoxResourceHandler
 * umi zpracovat dokument a vytvorit objekt mapy typu bedna.
 */

#ifndef BOX_RESOURCE_HANDLER_H_GUARD_
#define BOX_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicBox;

/** Resource handler specializovany na objekty typu box.
 * Zpracovava zdroj reprezentujici bednu a vytvari
 * prototyp teto bedny.
 */
class BoxResourceHandler: public MapObjectResourceHandler {

	public:
		/// Vytvorit prototyp zdi.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);
		/// Typ vytvareneho objektu.
		virtual BombicMapObject::Type type();
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);
};

#endif
