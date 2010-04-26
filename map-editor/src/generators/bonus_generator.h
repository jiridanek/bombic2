/** @file bonus_generator.h
 * Trida nesouci informace o generovani bonusu @c BonusGenerator.
 */

#ifndef BONUS_GENERATOR_H_GUARD_
#define BONUS_GENERATOR_H_GUARD_

#include "map_object_single_generator.h"

/** Generator bonusu.
 * Trida specializovana na generovani bonusu a jejich vizualizaci.
 */
class BonusGenerator: public MapObjectSingleGenerator {

	public:
		/// Konstrukce pro policko @p field.
		BonusGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();
};

#endif
