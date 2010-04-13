/** @file generated_creature.h
 * Trida nesouci informace o generovani priser @c BombicGeneratedCreature.
 */

#ifndef BOMBIC_GENERATED_CREATURE_H_GUARD_
#define BOMBIC_GENERATED_CREATURE_H_GUARD_

#include "generated_object.h"

/** Generovana prisera.
 * Trida  specializovana na vizualizaci generovani priser.
 */
class BombicGeneratedCreature: public BombicGeneratedObject {

	public:
		/// Konstrukce pro policko @p field.
		BombicGeneratedCreature(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();
};

#endif
