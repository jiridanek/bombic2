
#include "box_generator.h"

#include <QGraphicsSimpleTextItem>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v leve polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
BombicBoxGenerator::BombicBoxGenerator(const BombicMap::Field & field):
		BombicMapObjectGenerator(field) {
	labelGI_ = new QGraphicsSimpleTextItem("B");
	// TODO set font

	setLabelPos();
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type BombicBoxGenerator::type() {
	return BombicMapObject::Box;
}

/**
 * @return Zda lze aktualne generovat objekt.
 */
bool BombicBoxGenerator::canGenerate() {
	return BombicMapObjectGenerator::canGenerate() &&
		generatedObjects_.isEmpty();
}

void BombicBoxGenerator::addGeneratedObject(BombicMapObject * mapObject) {
	BombicMapObjectGenerator::addGeneratedObject(mapObject);
	emit canGenerateChanged();
}

bool BombicBoxGenerator::blocksCreatureGenerating() {
	return !generatedObjects_.isEmpty();
}
