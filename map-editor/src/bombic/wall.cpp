
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <constants.h>

#include "wall.h"

BombicWall::BombicWall(const QString & name, const QPixmap & pixmap,
		int width, int height, int toplapping):
				BombicMapObject(name, pixmap) {
	rect_.setSize(QSize(width, height));
	toplapping_ = toplapping;
}

BombicMapObject * BombicWall::createCopy() {
	return createWallCopy();
}
BombicWall * BombicWall::createWallCopy() {
	return new BombicWall(name_, pixmap_,
		rect_.width(), rect_.height(), toplapping_);
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
	qreal zValue = position.y() + (rect_.height()-1)*CELL_SIZE;
	if(isBackground()) {
		zValue -= 0.5;
	}
	graphicsItem_->setZValue(zValue);

	/* DEBUG
	foreach(QGraphicsItem * i, graphicsItem_->children()) {
		i->hide();
	}
	new QGraphicsSimpleTextItem(
		QString::number(zValue), graphicsItem_);
	// */

	return graphicsItem_;
}

bool BombicWall::canBeWith(BombicMapObject * object) {
	// wall can be on the same field only with another wall
	if(object->type() == Wall) {
		BombicWall * wallObj = static_cast<BombicWall *>(object);
		// but only if exactly one of them is background wall
		return (wallObj->isBackground() && !this->isBackground())
			|| (!wallObj->isBackground() && this->isBackground());
	} else {
		return false;
	}
}

bool BombicWall::isBackground() {
	// anonymous (with no name) is the background wall
	return name_.isEmpty();
}