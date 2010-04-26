/** @file map_object_single_generator.h
 * Trida generujici maximalne jeden objekt @c MapObjectSingleGenerator.
 */

#ifndef MAP_OBJECT_SINGLE_GENERATOR_H_GUARD_
#define MAP_OBJECT_SINGLE_GENERATOR_H_GUARD_

#include "map_object_generator.h"

/** Generator, ktery umoznuje generovat jen jeden objekt.
 */
class MapObjectSingleGenerator: public MapObjectGenerator {

	public:
		/// Konstrukce pro policko @p field.
		MapObjectSingleGenerator(const BombicMap::Field & field);

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type() =0;

		/// Jestli je mozne generovat dalsi objekt.
		virtual bool canGenerate();
		/// Pridat generovany objekt.
		virtual void addGeneratedObject(BombicMapObject * mapObject);
};

#endif
