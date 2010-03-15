/** @file map_object.h
 */

#ifndef MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_
#define MAP_OBJECT_RESOURCE_HANDLER_H_GUARD_

#include <QString>

class BombicMapObject;

class MapObjectResourceHandler {

	public:
		explicit MapObjectResourceHandler(const QString & name);
		virtual BombicMapObject * createMapObject(
				const QString & filename) = 0;
	protected:
		QString name_;
};

#endif
