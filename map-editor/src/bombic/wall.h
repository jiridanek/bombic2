/** @file wall.h
 */

#ifndef BOMBIC_WALL_H_GUARD_
#define BOMBIC_WALL_H_GUARD_

#include "map_object.h"

class BombicWall: public BombicMapObject {

	public:
		BombicWall(const QString & name, const QPixmap & pixmap,
				int width, int height, int toplapping);

		virtual BombicMapObject::Type type();

		virtual QGraphicsItem * createGraphicsItem(
				const QPointF & position);

		virtual bool canBeWith(BombicMapObject * object);

	private:
		int toplapping_;
};

#endif
