/** @file box_generator.h
 * Trida nesouci informace o generovani beden @c BoxGenerator.
 */

#ifndef BOX_GENERATOR_H_GUARD_
#define BOX_GENERATOR_H_GUARD_

#include "map_object_single_generator.h"

/** Generator beden.
 * Trida specializovana na generovani beden a jejich vizualizaci.
 */
class BoxGenerator: public MapObjectSingleGenerator {

	public:
		/// Konstrukce pro policko @p field.
		BoxGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

		/// Jestli objekt aktualne blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
		/// Jestli objekt aktualne odblokovava generovani bonusu.
		virtual bool unblocksBonusGenerating();

};

#endif
