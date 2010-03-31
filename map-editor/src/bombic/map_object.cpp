
#include "map_object.h"

#include <QGraphicsPixmapItem>
#include <constants.h>

BombicMapObject::BombicMapObject(const QString & name, const QPixmap & pixmap):
		name_(name), pixmap_(pixmap),
		thumbnail_(/* init it to null, it will be created on demand*/),
		graphicsItem_(0), rect_(0, 0, 1, 1), toplapping_(0) {
}

BombicMapObject::BombicMapObject(BombicMapObject & srcObject):
		name_(srcObject.name()), pixmap_(srcObject.pixmap()),
		thumbnail_(srcObject.thumbnail()),
		graphicsItem_(0), rect_(srcObject.rect()),
		toplapping_(srcObject.toplapping()) {
}

BombicMapObject::~BombicMapObject() {
	if(graphicsItem_) {
		// there is some item - delete it
		delete graphicsItem_;
	}
}

const QString & BombicMapObject::name() {
	return name_;
}

const QPixmap & BombicMapObject::pixmap() {
	return pixmap_;
}

const QPixmap & BombicMapObject::thumbnail() {
	if(thumbnail_.isNull()) {
		thumbnail_ = pixmap_.scaledToHeight(CELL_SIZE);
	}
	return thumbnail_;
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
	Q_UNUSED(object);
	return false;
}

bool BombicMapObject::blocksBoxGenerating() {
	return true;
}

bool BombicMapObject::blocksCreatureGenerating() {
	return true;
}

bool BombicMapObject::canBeDragged() {
	return true;
}

bool BombicMapObject::canBeRemoved() {
	return true;
}
