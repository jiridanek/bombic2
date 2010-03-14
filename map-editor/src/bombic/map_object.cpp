
#include "map_object.h"

BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap) {
}

BombicMapObject::~BombicMapObject() {
}

const QPixmap & BombicMapObject::pixmap() {
	return pixmap_;
}
