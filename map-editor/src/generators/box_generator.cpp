
#include "box_generator.h"

#include <QGraphicsSimpleTextItem>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v leve polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
BoxGenerator::BoxGenerator(const BombicMap::Field & field):
		MapObjectSingleGenerator(field) {
	labelGI_ = new QGraphicsSimpleTextItem("B");
	// TODO set font

	setLabelPos();
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type BoxGenerator::type() {
	return BombicMapObject::Box;
}

/**
 * @return Zda aktualne blokuje generovani priser.
 */
bool BoxGenerator::blocksCreatureGenerating() {
	return !generatedObjects_.isEmpty();
}

/**
 * @return Zda odblokovava generovani bonusu.
 */
bool BoxGenerator::unblocksBonusGenerating() {
	return !generatedObjects_.isEmpty();
}
