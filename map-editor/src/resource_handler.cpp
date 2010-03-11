
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

BombicMapObject * ResourceHandler::loadMapObject() {
}

BombicMap * ResourceHandler::loadMapFromFile(const QString & filename) {
}

BombicMap * ResourceHandler::loadMap() {
}

void ResourceHandler::saveMap(BombicMap * bombicMap) {
}

void ResourceHandler::saveMapAs(BombicMap * bombicMap) {
}
