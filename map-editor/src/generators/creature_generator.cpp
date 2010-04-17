
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
CreatureGenerator::CreatureGenerator(
		const BombicMap::Field & field):
				MapObjectGenerator(field) {
	labelGI_ = new QGraphicsSimpleTextItem("C");
	// TODO set font

	setLabelPos();
	// move to the right half of field
	labelGI_->moveBy(CELL_SIZE/2, 0);
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type CreatureGenerator::type() {
	return BombicMapObject::Creature;
}
