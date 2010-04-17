/** @file creature_generator.h
 * TODO Trida nesouci informace o generovani priser @c BombicCreatureGenerator.
 */

#ifndef CREATURE_GENERATOR_H_GUARD_
#define CREATURE_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generator priser.
 * TODO Trida  specializovana na vizualizaci generovani priser.
 */
class BombicCreatureGenerator: public BombicMapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		BombicCreatureGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

};

#endif
