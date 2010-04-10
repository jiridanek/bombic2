/** @file wall.h
 */

#ifndef WALL_RESOURCE_HANDLER_H_GUARD_
#define WALL_RESOURCE_HANDLER_H_GUARD_

#include <QString>

#include "map_object_resource_handler.h"

class BombicWall;

class WallResourceHandler: public MapObjectResourceHandler {

	public:
		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl);

		BombicWall * createWall(const QString & name,
			const QDomElement & imgEl);

		virtual BombicMapObject::Type type();

		/// Zda umi nacist objekt reprezentovany takovym XML elementem.
		virtual bool canHandle(const QDomElement & rootEl);

};

#endif
