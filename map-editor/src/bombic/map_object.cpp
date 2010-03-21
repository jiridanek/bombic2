
#include "map_object.h"

#include <QGraphicsPixmapItem>

BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap),
		graphicsItem_(0), size_(1, 1) {
}

BombicMapObject::~BombicMapObject() {
	if(graphicsItem_ && !graphicsItem_->scene()) {
		// there is some item but it is not in scene
		// I delete it
		delete graphicsItem_;
	}
}

const QString & BombicMapObject::name() {
	return name_;
}

const QPixmap & BombicMapObject::pixmap() {
	return pixmap_;
}

const QSize & BombicMapObject::size() {
	return size_;
}

QGraphicsItem * BombicMapObject::graphicsItem() {
	if(graphicsItem_) {
		return graphicsItem_;
	} else {
		return situateGraphicsItem(QPointF(0, 0));
	}
}

bool BombicMapObject::canBeWith(BombicMapObject * object) {
	return false;
}

