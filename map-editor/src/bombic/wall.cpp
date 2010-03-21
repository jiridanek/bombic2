
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "wall.h"

BombicWall::BombicWall(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap),
				toplapping_(toplapping) {
	size_ = QSize(width, height);
}

BombicMapObject * BombicWall::createCopy() {
	return createWallCopy();
}
BombicWall * BombicWall::createWallCopy() {
	return new BombicWall(name_, pixmap_,
		size_.width(), size_.height(), toplapping_);
}


BombicMapObject::Type BombicWall::type() {
	return Wall;
}

QGraphicsItem * BombicWall::situateGraphicsItem(const QPointF & position) {
	if(!graphicsItem_) {
		// create new one
		// it will be destroyed by scene or in BombicMapObject destructor
		graphicsItem_ = new QGraphicsPixmapItem(pixmap_);
		graphicsItem_->setOffset(0, -toplapping_*CELL_SIZE);
	}
	graphicsItem_->setPos(position);
	graphicsItem_->setZValue(position.y());
	return graphicsItem_;
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