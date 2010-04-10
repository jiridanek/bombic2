/** @file wall.h
 * Resource handler specializovany na zdi WallResourceHandler
 * umi zpracovat dokument a vytvorit objekt mapy typu zed.
 */

#ifndef WALL_RESOURCE_HANDLER_H_GUARD_
#define WALL_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicWall;

/** Resource handler specializovany na objekty typu zed.
 * Zpracovava zdroj reprezentujici zed a vytvari
 * prototyp teto zdi.
 */
class WallResourceHandler: public MapObjectResourceHandler {

	public:
		/// Vytvorit prototyp zdi.
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);
		/// Vytvorit prototyp zdi.
		BombicWall * createWall(const QString & name,
			const QDomElement & imgEl);
		/// Typ vytvareneho objektu.
		virtual BombicMapObject::Type type();
		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);
};

#endif
