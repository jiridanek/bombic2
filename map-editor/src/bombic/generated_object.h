/** @file generated_object.h
 */

#ifndef BOMBIC_GENERATED_OBJECT_H_GUARD_
#define BOMBIC_GENERATED_OBJECT_H_GUARD_

#include "map.h"

class QGraphicsItem;

class BombicGeneratedObject {

	public:
		~BombicGeneratedObject();

		QGraphicsItem * graphicsItem();

	protected:
		void setPos(const BombicMap::Field & field);

		QGraphicsItem * graphicsItem_;
};

#endif
