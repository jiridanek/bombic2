/** @file box_generator.h
 * Trida nesouci informace o generovani beden @c BoxGenerator.
 */

#ifndef BOX_GENERATOR_H_GUARD_
#define BOX_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generovana bedna.
 * Trida  specializovana na vizualizaci generovani beden.
 */
class BoxGenerator: public MapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		BoxGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

		virtual bool canGenerate();

		virtual void addGeneratedObject(BombicMapObject * mapObject);

		/// Jestli objekt aktualne blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
};

#endif
