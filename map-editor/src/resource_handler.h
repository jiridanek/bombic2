/** @file resource_handler.h
 */

#ifndef RESOURCE_HANDLER_H_GUARD_
#define RESOURCE_HANDLER_H_GUARD_

#include <QObject>
#include <singleton.h>

#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)


class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class ResourceHandler: public QObject {

	Q_OBJECT

	SINGLETON_DEFINITION(ResourceHandler)

	public:
		ResourceHandler(QObject * parent = 0);
		~ResourceHandler();

		BombicMap * loadMap();

		void saveMap(BombicMap * bombicMap);

		void saveMapAs(BombicMap * bombicMap);

		BombicMapBackground * loadMapBackground(const QString & name);

		BombicMapObject * loadMapObject();

		BombicMapObject * loadMapObject(const QString & name);
};

#endif
