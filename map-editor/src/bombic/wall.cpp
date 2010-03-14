
#include <QGraphicsItem>
#include <constants.h>

#include "wall.h"

BombicWall::BombicWall(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap),
				size_(width, height), toplapping_(toplapping) {
}

BombicMapObject::Type BombicWall::type() {
	return Wall;
}

QGraphicsItem * BombicWall::createGraphicsItem() {
	QGraphicsPixmapItem * gi = new QGraphicsPixmapItem(pixmap_);
	gi->setOffset(0, -toplapping_);
	return gi;
}