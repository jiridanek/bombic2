
#include "creature_generator.h"

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QPointF>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v prave polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
BombicCreatureGenerator::BombicCreatureGenerator(
		const BombicMap::Field & field):
				BombicMapObjectGenerator(field) {
	labelGI_ = new QGraphicsSimpleTextItem("C");
	// TODO set font

	setLabelPos();
	// move to the right half of field
	labelGI_->moveBy(CELL_SIZE/2, 0);
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type BombicCreatureGenerator::type() {
	return BombicMapObject::Creature;
}