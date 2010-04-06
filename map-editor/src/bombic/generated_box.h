/** @file generated_box.h
 * Trida nesouci informace o generovani beden @c BombicGeneratedCreature.
 */

#ifndef BOMBIC_GENERATED_BOX_H_GUARD_
#define BOMBIC_GENERATED_BOX_H_GUARD_

#include "generated_object.h"

/** Generovana bedna.
 * Trida  specializovana na vizualizaci generovani beden.
 */
class BombicGeneratedBox: public BombicGeneratedObject {

	public:
		/// Konstrukce pro policko @p field.
		BombicGeneratedBox(const BombicMap::Field & field);
};

#endif
