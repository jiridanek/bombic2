/** @file creature_generator.h
 * Trida nesouci informace o generovani priser @c CreatureGenerator.
 */

#ifndef CREATURE_GENERATOR_H_GUARD_
#define CREATURE_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generator priser.
 * Trida specializovana na generovani priser a jejich vizualizaci.
 */
class CreatureGenerator: public MapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		CreatureGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

};

#endif
