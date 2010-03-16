
#include "map_object.h"

BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap), size_(1, 1) {
}

BombicMapObject::~BombicMapObject() {
}

const QPixmap & BombicMapObject::pixmap() {
	return pixmap_;
}

const QSize & BombicMapObject::size() {
	return size_;
}

bool BombicMapObject::canBeWith(BombicMapObject * object) {
	return false;
}