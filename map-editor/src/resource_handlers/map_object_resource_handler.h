/** @file map_object.h
 */

#ifndef MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_
#define MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_

#include <QDomElement>
#include "bombic/map_object.h"

class MapObjectResourceHandler {

	public:
		static MapObjectResourceHandler * createResourceHandler(
				const QDomElement & rootEl);

		virtual BombicMapObject * createMapObject(
				const QDomElement & rootEl) = 0;

		virtual BombicMapObject::Type type() = 0;
};

#endif
