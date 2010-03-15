/** @file wall.h
 */

#ifndef WALL_RESOURCE_HANDLER_H_GUARD_
#define WALL_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include <QString>

#include "map_object_resource_handler.h"

class BombicWall;

class WallResourceHandler: public MapObjectResourceHandler {

	public:
		explicit WallResourceHandler(const QString & name);
		virtual BombicMapObject * createMapObject(
				const QString & filename);
		BombicWall * createWall(const QDomElement & imgEl);
};

#endif
