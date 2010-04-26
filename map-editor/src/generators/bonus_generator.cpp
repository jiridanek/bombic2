
#include "bonus_generator.h"

/** @details
 * Nevytvari graficky prvek vizualizace.
 * Generator bonusu je by default blokovany, nekdo (bedna) jej musi odblokovat.
 * @param field policko mapy, pro ktere chceme generovat
 */
BonusGenerator::BonusGenerator(const BombicMap::Field & field):
		MapObjectSingleGenerator(field) {

	block();
}

/**
 * @return Typ objektu, ktery generuje.
 */
BombicMapObject::Type BonusGenerator::type() {
	return BombicMapObject::Bonus;
}
