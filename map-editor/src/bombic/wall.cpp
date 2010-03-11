
#include "wall.h"

BombicWall::BombicWall(const QString & name, const QPixmap & pixmap):
		BombicMapObject(name, pixmap) {
}

BombicMapObject::Type BombicWall::type() {
	return Wall;
}
