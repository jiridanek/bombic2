
#include "resource_handler.h"

#include "bombic/map.h"
#include "bombic/map_object.h"

SINGLETON_INIT(ResourceHandler);

ResourceHandler::ResourceHandler(QObject * parent):
		QObject(parent) {
	SINGLETON_CONSTRUCT;
}

ResourceHandler::~ResourceHandler() {
	SINGLETON_DESTROY;
}

BombicMap * ResourceHandler::loadMap() {
}

BombicMapBackground * ResourceHandler::loadMapBackground(
		const QString & name) {
	
}

BombicMapObject * ResourceHandler::loadMapObject() {
}

BombicMapObject * ResourceHandler::loadMapObject(const QString & name) {
}

void ResourceHandler::saveMap(BombicMap * bombicMap) {
}

void ResourceHandler::saveMapAs(BombicMap * bombicMap) {
}
