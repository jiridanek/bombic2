/** @file creature_generator.h
 * TODO Trida nesouci informace o generovani priser @c CreatureGenerator.
 */

#ifndef CREATURE_GENERATOR_H_GUARD_
#define CREATURE_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generator priser.
 * TODO Trida  specializovana na vizualizaci generovani priser.
 */
class CreatureGenerator: public MapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		CreatureGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

};

#endif
