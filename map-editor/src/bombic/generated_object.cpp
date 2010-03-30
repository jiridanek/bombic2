
#include "generated_object.h"

#include <QGraphicsItem>
#include <constants.h>

BombicGeneratedObject::~BombicGeneratedObject(){
	delete graphicsItem_;
}

QGraphicsItem * BombicGeneratedObject::graphicsItem() {
	return graphicsItem_;
}

void BombicGeneratedObject::setPos(const BombicMap::Field & field) {
	// move to the field
	graphicsItem_->setPos(field*CELL_SIZE);
	// move by offset
	graphicsItem_->moveBy(
		( CELL_SIZE/2 - graphicsItem_->boundingRect().width() )/2.0, 3);
}