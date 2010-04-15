/** @file box_generator.h
 * Trida nesouci informace o generovani beden @c BombicBoxGenerator.
 */

#ifndef BOMBIC_BOX_GENERATOR_H_GUARD_
#define BOMBIC_BOX_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generovana bedna.
 * Trida  specializovana na vizualizaci generovani beden.
 */
class BombicBoxGenerator: public BombicMapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		BombicBoxGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type();

		virtual bool canGenerate();

		virtual void addGeneratedObject(BombicMapObject * mapObject);

		/// Jestli objekt aktualne blokuje generovani priser.
		virtual bool blocksCreatureGenerating();
};

#endif
