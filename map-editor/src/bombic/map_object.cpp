
#include "map_object.h"

#include <QGraphicsPixmapItem>

BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap),
		graphicsItem_(0), rect_(0, 0, 1, 1), toplapping_(0) {
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

const QRect & BombicMapObject::rect() {
	return rect_;
}

BombicMap::Field BombicMapObject::field() {
	return rect_.topLeft();
}

void BombicMapObject::setField(const BombicMap::Field & field) {
	rect_.moveTopLeft(field);
}
QSize BombicMapObject::size() {
	return rect_.size();
}

int BombicMapObject::toplapping() {
	return toplapping_;
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

