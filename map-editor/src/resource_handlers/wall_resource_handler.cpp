
#include <QPixmap>

#include <constants.h>

#include "wall_resource_handler.h"

#include "../resource_handler.h"
#include "../bombic/map_object.h"
#include "../bombic/wall.h"


BombicMapObject * WallResourceHandler::createMapObject(
		const QDomElement & rootEl) {
	QDomElement imgEl;
	if(RESOURCE_HANDLER->getSubElement(rootEl, imgEl)) {
		return createWall(rootEl.attribute("name"), imgEl);
	} else {
		return 0;
	}
}

BombicWall * WallResourceHandler::createWall(const QString & name,
		const QDomElement & imgEl) {
	// get attributes
	int x = 0;
	int y = 0;
	int w = 1;
	int h = 1;
	int t = 0; // toplapping
	bool success =
		RESOURCE_HANDLER->getAttrsXY(imgEl, x, y) &&
		RESOURCE_HANDLER->getIntAttr(imgEl, w, "width", true) &&
		RESOURCE_HANDLER->getIntAttr(imgEl, h, "height", true) &&
		RESOURCE_HANDLER->getIntAttr(imgEl, t, "toplapping", true);
	if(!success) {
		return 0;
	}
	// get pixmap
	QPixmap pixmap = RESOURCE_HANDLER->sourcePixmap_.copy(
		x, y, w*CELL_SIZE, (h+t)*CELL_SIZE);

	return new BombicWall(name, pixmap, w, h, t);
}

BombicMapObject::Type WallResourceHandler::type() {
	return BombicMapObject::Wall;
}