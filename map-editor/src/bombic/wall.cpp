
#include <QGraphicsItem>
#include <constants.h>

#include "wall.h"

BombicWall::BombicWall(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap),
				toplapping_(toplapping) {
	size_ = QSize(width, height);
}

BombicMapObject::Type BombicWall::type() {
	return Wall;
}

QGraphicsItem * BombicWall::createGraphicsItem(const QPointF & position) {
	QGraphicsPixmapItem * gi = new QGraphicsPixmapItem(pixmap_);
	gi->setOffset(0, -toplapping_*CELL_SIZE);
	gi->setPos(position);
	return gi;
}

bool BombicWall::canBeWith(BombicMapObject * object) {
	// wall can be on the same field only with another wall
	if(object->type() == Wall) {
		BombicWall * wallObj = static_cast<BombicWall *>(object);
		// but only if right one of them is anonymous (background) wall
		return (wallObj->name_.isEmpty() && !this->name_.isEmpty())
			|| (!wallObj->name_.isEmpty() && this->name_.isEmpty());
	} else {
		return false;
	}
}