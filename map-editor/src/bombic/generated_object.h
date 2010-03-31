/** @file generated_object.h
 */

#ifndef BOMBIC_GENERATED_OBJECT_H_GUARD_
#define BOMBIC_GENERATED_OBJECT_H_GUARD_

#include "map.h"

class QGraphicsItem;

class BombicGeneratedObject {

	public:
		BombicGeneratedObject();
		~BombicGeneratedObject();

		void show();
		void hide();
		void allow();
		void disallow();

		bool allowed();

		QGraphicsItem * graphicsItem();

	protected:
		void setPos(const BombicMap::Field & field);

		QGraphicsItem * graphicsItem_;

		bool allowed_;
		bool hidden_;
};

#endif
