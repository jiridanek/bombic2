
#include "generated_box.h"

#include <QGraphicsSimpleTextItem>
#include <constants.h>

BombicGeneratedBox::BombicGeneratedBox(const BombicMap::Field & field) {
	graphicsItem_ = new QGraphicsSimpleTextItem("B");
	// TODO set font

	setPos(field);
}
