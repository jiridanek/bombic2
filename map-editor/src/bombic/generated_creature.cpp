
#include "generated_creature.h"

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QPointF>
#include <constants.h>

BombicGeneratedCreature::BombicGeneratedCreature(const BombicMap::Field & field) {
	graphicsItem_ = new QGraphicsSimpleTextItem("C");
	// TODO set font

	setPos(field);
	// move to the right half of field
	graphicsItem_->moveBy(CELL_SIZE/2, 0);
}
