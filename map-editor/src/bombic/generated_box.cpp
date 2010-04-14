
#include "generated_box.h"

#include <QGraphicsSimpleTextItem>
#include <constants.h>

/** @details
 * Vytvori graficky prvek vizualizace pro policko @p field,
 * ktery se nachazi v leve polovine tohoto policka.
 * @param field policko mapy, pro ktere chceme vizualizovat
 */
BombicGeneratedBox::BombicGeneratedBox(const BombicMap::Field & field):
		BombicGeneratedObject(field) {
	labelGI_ = new QGraphicsSimpleTextItem("B");
	// TODO set font

	setLabelPos();
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type BombicGeneratedBox::type() {
	return BombicMapObject::Box;
}

/**
 * @return Zda lze aktualne generovat objekt.
 */
bool BombicGeneratedBox::canGenerate() {
	return BombicGeneratedObject::canGenerate() &&
		generatedObjects_.isEmpty();
}

void BombicGeneratedBox::addGeneratedObject(BombicMapObject * mapObject) {
	BombicGeneratedObject::addGeneratedObject(mapObject);
	emit canGenerateChanged();
}

bool BombicGeneratedBox::blocksCreatureGenerating() {
	return !generatedObjects_.isEmpty();
}
