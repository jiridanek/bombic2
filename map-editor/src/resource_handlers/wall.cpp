
#include <constants.h>

#include "../resource_handler.h"
#include "../bombic/map_object.h"
#include "../bombic/wall.h"

WallResourceHandler::WallResourceHandler(const QString & name):
		MapObjectResourceHandler(name) {
}

BombicMapObject * WallResourceHandler::createMapObject(
		const QString & filename) {

}

BombicWall * WallResourceHandler::createWall(const QDomElement & imgEl) {
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
		x, y - t*CELL_SIZE, w*CELL_SIZE, (h+t)*CELL_SIZE);

	return new BombicWall(name_, pixmap, w, h, t);
}
